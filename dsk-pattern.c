#include <string.h>
#include "dsk.h"
#include "dsk-list-macros.h"
#include "dsk-rbtree-macros.h"
#include "dsk-qsort-macro.h"

#define DEBUG_TOKENIZE    0
#define DEBUG_PARSE       0
#define DEBUG_NFA         0
#define DEBUG_DFA         0

#define IMPLEMENT_TRUMP_STATES   0

#ifndef MIN
#define MIN(a,b)   ((a) < (b) ? (a) : (b))
#endif

struct CharClass
{
  uint8_t set[32];
};
#define MK_LITERAL_CHAR_CLASS(c) ((struct CharClass *) (size_t) (uint8_t) (c))
#define IS_SINGLE_CHAR_CLASS(cl) (((size_t)(cl)) < 256)
#define CHAR_CLASS_BITVEC_SET(cl, v)    cl->set[((uint8_t)(v))/8] |= (1 << ((uint8_t)(v) % 8))
#define SINGLE_CHAR_CLASS_GET_CHAR(cl)     ((uint8_t)(size_t)(cl))
#define CHAR_CLASS_BITVEC_IS_SET(cl, v)    ((cl->set[((uint8_t)(v))/8] & (1 << ((uint8_t)(v) % 8))) != 0)
#define CHAR_CLASS_IS_SET(cl, v)                 \
                     ( IS_SINGLE_CHAR_CLASS (cl) \
                       ? (v) == SINGLE_CHAR_CLASS_GET_CHAR (trans->char_class) \
                       : CHAR_CLASS_BITVEC_IS_SET (trans->char_class, (v)) )

static struct CharClass char_class_dot = {{
    254,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255
}};

static void
char_class_union_inplace (struct CharClass *inout, const struct CharClass *addend)
{
  unsigned i;
  if (IS_SINGLE_CHAR_CLASS (addend))
    CHAR_CLASS_BITVEC_SET (inout, SINGLE_CHAR_CLASS_GET_CHAR (addend));
  else
    for (i = 0; i < DSK_N_ELEMENTS (inout->set); i++)
      inout->set[i] |= addend->set[i];
}
static void
char_class_reverse_inplace (struct CharClass *inout)
{
  unsigned i;
  inout->set[0] ^= 254;         /* NUL is omitted */
  for (i = 1; i < 32; i++)
    inout->set[i] ^= 255;
}

#if DEBUG_NFA || DEBUG_PARSE || DEBUG_TOKENIZE
#include <stdio.h>
static void
print_char_class (const struct CharClass *cclass)
{
  if (IS_SINGLE_CHAR_CLASS (cclass))
    printf ("%s", dsk_ascii_byte_name (SINGLE_CHAR_CLASS_GET_CHAR (cclass)));
  else
    {
      dsk_boolean first = DSK_TRUE;
      unsigned i,j;
      for (i = 1; i < 256; )
        if (CHAR_CLASS_BITVEC_IS_SET (cclass, i))
          {
            j = i;
            while ((j+1) < 256 && CHAR_CLASS_BITVEC_IS_SET (cclass, j+1))
              j++;
            if (first)
              first = DSK_FALSE;
            else
              printf (" ");
            if (i == j)
              printf ("%s", dsk_ascii_byte_name (i));
            else
              printf ("%s-%s", dsk_ascii_byte_name (i), dsk_ascii_byte_name (j));
            i = j + 1;
          }
        else
          i++;
    }
}
#endif

#define COMPARE_INT(a,b)   ((a)<(b) ? -1 : (a)>(b) ? 1 : 0)

static int
compare_char_class_to_single (const struct CharClass *a,
                              uint8_t                 b)
{
  const uint8_t *A = a->set;
  unsigned i;
  unsigned v;
  for (i = 0; i < b/8U; i++)
    if (A[i] > 0)
      return 1;
  v = 1 << (b%8);
  if (A[i] < v)
    return -1;
  if (A[i] > v)
    return +1;
  for (i++; i < 32; i++)
    if (A[i] > 0)
      return 1;
  return 0;
}

static int
compare_char_classes (const struct CharClass *a,
                      const struct CharClass *b)
{
  if (IS_SINGLE_CHAR_CLASS (a))
    {
      if (IS_SINGLE_CHAR_CLASS (b))
        return COMPARE_INT (SINGLE_CHAR_CLASS_GET_CHAR (a),
                            SINGLE_CHAR_CLASS_GET_CHAR (b));
      else
        return -compare_char_class_to_single (b, SINGLE_CHAR_CLASS_GET_CHAR (a));
    }
  else
    {
      if (IS_SINGLE_CHAR_CLASS (b))
        return compare_char_class_to_single (b, SINGLE_CHAR_CLASS_GET_CHAR (a));
      else
        return memcmp (a->set, b->set, sizeof (struct CharClass));
    }
}

/* --- Pattern -- A Parsed Regex --- */
typedef enum
{
  PATTERN_LITERAL,
  PATTERN_CONCAT,               /* concatenation */
  PATTERN_ALT,                  /* alternation */
  PATTERN_OPTIONAL,             /* optional pattern */
  PATTERN_STAR,                 /* repeated 0 or more times */
  PATTERN_PLUS,                 /* repeated 1 or more times */
  PATTERN_EMPTY
} PatternType;

struct Pattern
{
  PatternType type;
  union
  {
    struct CharClass *literal;
    struct { struct Pattern *a, *b; } concat;
    struct { struct Pattern *a, *b; } alternation;
    struct Pattern *optional;
    struct Pattern *star;
    struct Pattern *plus;
  } info;
};
#if DEBUG_PARSE
static void
dump_pattern (struct Pattern *pattern, unsigned indent)
{
  printf ("%*s", indent, "");
  switch (pattern->type)
    {
    case PATTERN_LITERAL:
      printf ("literal: ");
      print_char_class (pattern->info.literal);
      printf ("\n");
      break;
    case PATTERN_CONCAT:
      printf ("concat:\n");
      dump_pattern (pattern->info.concat.a, indent+1);
      dump_pattern (pattern->info.concat.b, indent+1);
      break;
    case PATTERN_ALT:
      printf ("alternation:\n");
      dump_pattern (pattern->info.alternation.a, indent+1);
      dump_pattern (pattern->info.alternation.b, indent+1);
      break;
    case PATTERN_OPTIONAL:
      printf ("optional:\n");
      dump_pattern (pattern->info.optional, indent+1);
      break;
    case PATTERN_STAR:
      printf ("star:\n");
      dump_pattern (pattern->info.optional, indent+1);
      break;
    case PATTERN_PLUS:
      printf ("plus:\n");
      dump_pattern (pattern->info.optional, indent+1);
      break;
    case PATTERN_EMPTY:
      printf ("[empty]\n");
    }
}
#endif

typedef enum
{
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_ALTER,
  TOKEN_STAR,
  TOKEN_PLUS,
  TOKEN_QUESTION_MARK,
  TOKEN_PATTERN         /* whilst parsing */
} TokenType;

struct Token
{
  TokenType type;
  struct Pattern *pattern;
  struct Token *prev, *next;
};

#if DEBUG_TOKENIZE
static void
dump_token_list (struct Token *token)
{
  while (token)
    {
      printf ("token %p: ", token);
      switch (token->type)
        {
        case TOKEN_LPAREN: printf ("("); break;
        case TOKEN_RPAREN: printf (")"); break;
        case TOKEN_ALTER:  printf ("|"); break;
        case TOKEN_STAR:   printf ("*"); break;
        case TOKEN_PLUS:   printf ("+"); break;
        case TOKEN_QUESTION_MARK: printf ("?"); break;
        case TOKEN_PATTERN:
          dsk_assert (token->pattern->type == PATTERN_LITERAL);
          print_char_class (token->pattern->info.literal);
          break;
        }
      printf ("\n");
      token = token->next;
    }
}
#endif

static struct {
  char c;
  struct CharClass cclass;
} special_char_classes[] = {
#include "dsk-pattern-char-classes.inc"
};

/* Parse a backslash-escape special character class,
   include a lot of punctuation.  The backslash
   should already have been skipped. */
static dsk_boolean
get_backslash_char_class (const char **p_at, struct CharClass **out)
{
  const char *start = *p_at;
  char bs = *start;
  if (dsk_ascii_ispunct (bs))
    *out = MK_LITERAL_CHAR_CLASS (bs);
  else if (bs == 'n')
    *out = MK_LITERAL_CHAR_CLASS ('\n');
  else if (bs == 'r')
    *out = MK_LITERAL_CHAR_CLASS ('\r');
  else if (bs == 't')
    *out = MK_LITERAL_CHAR_CLASS ('\t');
  else if (bs == 'v')
    *out = MK_LITERAL_CHAR_CLASS ('\v');
  else if (dsk_ascii_isdigit (bs))
    {
      uint8_t value;
      if (!dsk_ascii_isdigit (start[1]))
        {
          value = start[0] - '0';
          *p_at = start + 1;
        }
      else if (!dsk_ascii_isdigit (start[2]))
        {
          value = (start[0] - '0') * 8 + (start[1] - '0');
          *p_at = start + 2;
        }
      else
        {
          value = (start[0] - '0') * 64 + (start[1] - '0') * 8 + (start[2] - '0');
          *p_at = start + 3;
        }
      *out = MK_LITERAL_CHAR_CLASS (value);
      return DSK_TRUE;
    }
  else
    {
      unsigned i;
      for (i = 0; i < DSK_N_ELEMENTS (special_char_classes); i++)
        if (special_char_classes[i].c == bs)
          {
            *out = &special_char_classes[i].cclass;
            *p_at = start + 1;
            return DSK_TRUE;
          }
      return DSK_FALSE;
    }
  *p_at = start + 1;
  return DSK_TRUE;
}

/* Parse a [] character class expression */
static struct CharClass *
parse_character_class (const char **p_regex,
                       DskMemPool  *pool,
                       DskError   **error)
{
  const char *at = *p_regex;
  dsk_boolean reverse = DSK_FALSE;
  struct CharClass *out = dsk_mem_pool_alloc0 (pool, sizeof (struct CharClass));
  if (*at == '^')
    {
      reverse = DSK_TRUE;
      at++;
    }
  while (*at != 0 && *at != ']')
    {
      /* this muck is structured annoyingly:  we just to the label
         got_range_start_and_dash whenever we encounter a '-' after
         a single character (either literally or as a backslash sequence),
         to handle range expressions. */
      unsigned first_value;

      if (*at == '\\')
        {
          struct CharClass *sub;
          at++;
          if (!get_backslash_char_class (&at, &sub))
            {
              *p_regex = at;    /* for error reporting (maybe?) */
              dsk_set_error (error, "bad \\ expression (at %s)", dsk_ascii_byte_name (*at));
              return NULL;
            }
          if (IS_SINGLE_CHAR_CLASS (sub) && *at == '-')
            {
              first_value = SINGLE_CHAR_CLASS_GET_CHAR (sub);
              at++;
              goto got_range_start_and_dash;
            }
          char_class_union_inplace (out, sub);
        }
      else if (at[1] == '-')
        {
          first_value = *at;
          at += 2;
          goto got_range_start_and_dash;
        }
      else
        {
          /* single character */
          CHAR_CLASS_BITVEC_SET (out, *at);
          at++;
        }

      continue;
got_range_start_and_dash:
      {
        unsigned last_value;
        unsigned code;
        if (*at == '\\')
          {
            struct CharClass *sub;
            const char *start;
            at++;
            start = at;
            if (!get_backslash_char_class (&at, &sub))
              {
                *p_regex = at;    /* for error reporting (maybe?) */
                dsk_set_error (error, "bad \\ expression (at %s)", dsk_ascii_byte_name (*at));
                return NULL;
              }
            if (!IS_SINGLE_CHAR_CLASS (sub))
              {
                dsk_set_error (error, "non-single-byte \\%c encountered - cannot use in range", *start);
                return NULL;
              }
            last_value = SINGLE_CHAR_CLASS_GET_CHAR (sub);
          }
        else if (*at == ']')
          {
            /* syntax error */
            dsk_set_error (error, "unterminated character class range");
            return NULL;
          }
        else
          {
            last_value = *at;
            at++;
          }

        if (first_value > last_value)
          {
            dsk_set_error (error, "character range is not first<last (first=%s, last=%s)",
                           dsk_ascii_byte_name (first_value),
                           dsk_ascii_byte_name (last_value));
            return NULL;
          }
        for (code = first_value; code <= last_value; code++)
          CHAR_CLASS_BITVEC_SET (out, code);
      }
    }
  *p_regex = at;
  if (reverse)
    char_class_reverse_inplace (out);
  return out;
}

static dsk_boolean
tokenize (const char   *regex,
          struct Token **token_list_out,
          DskMemPool   *pool,
          DskError    **error)
{
  struct Token *last = NULL;
  *token_list_out = NULL;
  while (*regex)
    {
      struct Token *t = dsk_mem_pool_alloc (pool, sizeof (struct Token));
      switch (*regex)
        {
        case '*':
          t->type = TOKEN_STAR;
          regex++;
          break;
        case '+':
          t->type = TOKEN_PLUS;
          regex++;
          break;
        case '?':
          t->type = TOKEN_QUESTION_MARK;
          regex++;
          break;
        case '(':
          t->type = TOKEN_LPAREN;
          regex++;
          break;
        case ')':
          t->type = TOKEN_RPAREN;
          regex++;
          break;
        case '|':
          t->type = TOKEN_ALTER;
          regex++;
          break;
        case '[':
          {
            struct CharClass *cclass;
            /* parse character class */
            regex++;
            cclass = parse_character_class (&regex, pool, error);
            if (cclass == NULL || *regex != ']')
              return DSK_FALSE;
            regex++;
            t->type = TOKEN_PATTERN;
            t->pattern = dsk_mem_pool_alloc (pool, sizeof (struct Pattern));
            t->pattern->type = PATTERN_LITERAL;
            t->pattern->info.literal = cclass;
            break;
          }
        case '\\':
          {
            /* parse either char class or special literal */
            struct CharClass *cclass;
            regex++;
            if (get_backslash_char_class (&regex, &cclass))
              {
                t->type = TOKEN_PATTERN;
                t->pattern = dsk_mem_pool_alloc (pool, sizeof (struct Pattern));
                t->pattern->type = PATTERN_LITERAL;
                t->pattern->info.literal = cclass;
              }
            else
              {
                if (regex[1] == 0)
                  dsk_set_error (error, "unexpected backslash sequence in regex");
                else
                  dsk_set_error (error, "bad char %s after backslash", dsk_ascii_byte_name (regex[1]));
                return DSK_FALSE;
              }
            break;
          }
        case '.':
          t->type = TOKEN_PATTERN;
          t->pattern = dsk_mem_pool_alloc (pool, sizeof (struct Pattern));
          t->pattern->type = PATTERN_LITERAL;
          t->pattern->info.literal = &char_class_dot;
          regex++;
          break;
        default:
          /* character literal */
          t->type = TOKEN_PATTERN;
          t->pattern = dsk_mem_pool_alloc (pool, sizeof (struct Pattern));
          t->pattern->type = PATTERN_LITERAL;
          t->pattern->info.literal = MK_LITERAL_CHAR_CLASS (regex[0]);
          regex++;
          break;
        }

      /* append to list */
      t->prev = last;
      t->next = NULL;
      if (last)
        last->next = t;
      else
        *token_list_out = last = t;
      last = t;
    }
  return DSK_TRUE;
}

static struct Pattern *
parse_pattern (unsigned      pattern_index,
               struct Token *token_list,
               DskMemPool   *pool,
               DskError    **error)
{
  dsk_boolean last_was_alter;
  dsk_boolean accept_empty;

  /* Handle parens */
  struct Token *token;
  for (token = token_list; token; token = token->next)
    if (token->type == TOKEN_LPAREN)
      {
        /* find matching rparen (or error) */
        struct Token *rparen = token->next;
        int balance = 1;
        struct Pattern *subpattern;
        while (rparen)
          {
            if (rparen->type == TOKEN_LPAREN)
              balance++;
            else if (rparen->type == TOKEN_RPAREN)
              {
                balance--;
                if (balance == 0)
                  break;
              }
            rparen = rparen->next;
          }
        if (balance)
          {
            /* missing right-paren */
            dsk_set_error (error, "missing right-paren in regex");
            return NULL;
          }

        /* recurse */
        rparen->prev->next = NULL;
        subpattern = parse_pattern (pattern_index, token->next, pool, error);
        if (subpattern == NULL)
          return NULL;

        /* replace parenthesized expr with subpattern; slice out remainder of list */
        token->type = TOKEN_PATTERN;
        token->pattern = subpattern;
        token->next = rparen->next;
        if (rparen->next)
          token->next->prev = token;
      }
    else if (token->type == TOKEN_RPAREN)
      {
        dsk_set_error (error, "unexpected right-paren in regex");
        return NULL;
      }

  /* Handle star/plus/qm */
  for (token = token_list; token; token = token->next)
    if (token->type == TOKEN_QUESTION_MARK
     || token->type == TOKEN_STAR
     || token->type == TOKEN_PLUS)
      {
        struct Pattern *new_pattern;
        if (token->prev == NULL || token->prev->type != TOKEN_PATTERN)
          {
            dsk_set_error (error, "'%c' must be precede by pattern",
                           token->type == TOKEN_QUESTION_MARK ? '?' 
                           : token->type == TOKEN_STAR ? '*'
                           : '+');
            return NULL;
          }
        new_pattern = dsk_mem_pool_alloc (pool, sizeof (struct Pattern));
        switch (token->type)
          {
          case TOKEN_QUESTION_MARK:
            new_pattern->type = PATTERN_OPTIONAL;
            new_pattern->info.optional = token->prev->pattern;
            break;
          case TOKEN_STAR:
            new_pattern->type = PATTERN_STAR;
            new_pattern->info.star = token->prev->pattern;
            break;
          case TOKEN_PLUS:
            new_pattern->type = PATTERN_PLUS;
            new_pattern->info.plus = token->prev->pattern;
            break;
          default:
            dsk_assert_not_reached ();
          }
        token->prev->pattern = new_pattern;

        /* remove token */
        if (token->prev)
          token->prev->next = token->next;
        else
          token_list = token->next;
        if (token->next)
          token->next->prev = token->prev;
        /* token isn't in the list now!  but it doesn't matter b/c token->next is still correct */
      }

  /* Handle concatenation */
  for (token = token_list; token && token->next; )
    {
      if (token->type == TOKEN_PATTERN
       && token->next->type == TOKEN_PATTERN)
        {
          /* concat */
          struct Pattern *new_pattern = dsk_mem_pool_alloc (pool, sizeof (struct Pattern));
          struct Token *kill;
          new_pattern->type = PATTERN_CONCAT;
          new_pattern->info.concat.a = token->pattern;
          new_pattern->info.concat.b = token->next->pattern;
          token->pattern = new_pattern;

          /* remove token->next */
          kill = token->next;
          token->next = kill->next;
          if (kill->next)
            kill->next->prev = token;
        }
      else
        token = token->next;
    }

  /* At this point we consist of nothing but alternations and
     patterns.  Scan through, discarding TOKEN_ALTER,
     and keeping track of whether the empty pattern matches */
  last_was_alter = DSK_TRUE;     /* trick the empty pattern detector
                                    into triggering on initial '|' */
  accept_empty = DSK_FALSE;
  for (token = token_list; token; token = token->next)
    if (token->type == TOKEN_ALTER)
      {
        if (last_was_alter)
          accept_empty = DSK_TRUE;
        last_was_alter = DSK_TRUE;

        /* remove token from list */
        if (token->prev)
          token->prev->next = token->next;
        else
          token_list = token->next;
        if (token->next)
          token->next->prev = token->prev;
      }
    else
      {
        last_was_alter = DSK_FALSE;
      }
  if (last_was_alter)
    accept_empty = DSK_TRUE;

  /* if we accept an empty token, toss a PATTERN_EMPTY onto the list of patterns
     in the alternation. */
  if (accept_empty || token_list == NULL)
    {
      struct Token *t = dsk_mem_pool_alloc (pool, sizeof (struct Token));
      t->next = token_list;
      t->prev = NULL;
      if (t->next)
        t->next->prev = t;
      token_list = t;

      t->type = TOKEN_PATTERN;
      t->pattern = dsk_mem_pool_alloc (pool, sizeof (struct Pattern));
    }

  /* At this point, token_list!=NULL,
     and it consists entirely of patterns.
     Reduce it to a singleton with the alternation pattern. */
  while (token_list->next != NULL)
    {
      /* create alternation pattern */
      struct Pattern *new_pattern = dsk_mem_pool_alloc (pool, sizeof (struct Pattern));
      new_pattern->type = PATTERN_ALT;
      new_pattern->info.alternation.a = token_list->pattern;
      new_pattern->info.alternation.b = token_list->next->pattern;
      token_list->pattern = new_pattern;

      /* remove token->next */
      {
        struct Token *kill = token_list->next;
        token_list->next = kill->next;
        if (kill->next)
          kill->next->prev = token_list;
      }
    }

  /* Return value consists of merely a single token-list. */
  dsk_assert (token_list != NULL && token_list->next == NULL);
  return token_list->pattern;
}

/* --- Nondeterministic Finite Automata --- */
struct NFA_Transition
{
  struct CharClass *char_class;               /* if NULL then no char consumed */
  struct NFA_State *next_state;
  struct NFA_Transition *next_in_state;
};

struct NFA_State
{
  struct Pattern *pattern;
  struct NFA_Transition *transitions;
  unsigned is_match : 1;
  unsigned flag : 1;            /* multipurpose flag */
  unsigned pattern_index : 30;
};

#define NFA_STATE_TRANSITION_STACK(head) \
   struct NFA_Transition *, (head), next_in_state
#define NFA_STATE_GET_TRANSITION_STACK(state) \
   NFA_STATE_TRANSITION_STACK((state)->transitions)

static inline void
prepend_transition (struct NFA_Transition **plist,
                    struct CharClass       *char_class,
                    struct NFA_State       *next_state,
                    DskMemPool             *pool)
{
  struct NFA_Transition *t = dsk_mem_pool_alloc (pool, sizeof (struct NFA_Transition));
  t->char_class = char_class;
  t->next_state = next_state;
  t->next_in_state = *plist;
  *plist = t;
}

/* Converting pattern to NFS_State, allowing transitions that
   don't consume characters */
static struct NFA_State *
pattern_to_nfa_state (struct Pattern *pattern,
                      unsigned pattern_index,
                      struct NFA_State *result,
                      DskMemPool *pool)
{
  struct NFA_State *b, *rv;
  struct NFA_Transition *trans;
tail_recurse:
  switch (pattern->type)
    {
    case PATTERN_LITERAL:
      /* Allocate state with one transition */
      rv = dsk_mem_pool_alloc (pool, sizeof (struct NFA_State));
      rv->pattern_index = pattern_index;
      rv->pattern = pattern;
      rv->transitions = NULL;
      rv->flag = 0;
      rv->is_match = DSK_FALSE;
      prepend_transition (&rv->transitions, pattern->info.literal, result, pool);
      break;
      
    case PATTERN_ALT:
      rv = pattern_to_nfa_state (pattern->info.alternation.a, pattern_index, result, pool);
      b = pattern_to_nfa_state (pattern->info.alternation.b, pattern_index, result, pool);
      for (trans = b->transitions; trans; trans = trans->next_in_state)
        prepend_transition (&rv->transitions, trans->char_class, trans->next_state, pool);
      break;
    case PATTERN_CONCAT:
      /* NOTE: we handle tail_recursion to benefit long strings of concatenation
             (((('a' . 'b') . 'c') . 'd') . 'e')
         thus, the parser should be careful to arrange the concat patterns thusly */
      b = pattern_to_nfa_state (pattern->info.concat.b, pattern_index, result, pool);
      pattern = pattern->info.concat.a;
      result = b;
      goto tail_recurse;
    case PATTERN_OPTIONAL:
      rv = pattern_to_nfa_state (pattern->info.optional, pattern_index, result, pool);
      prepend_transition (&rv->transitions, NULL, result, pool);
      break;
    case PATTERN_PLUS:
      rv = pattern_to_nfa_state (pattern->info.plus, pattern_index, result, pool);
      prepend_transition (&result->transitions, NULL, rv, pool);
      break;
    case PATTERN_STAR:
      {
        struct NFA_State *new_result;
        new_result = dsk_mem_pool_alloc0 (pool, sizeof (struct NFA_State));
        new_result->pattern = pattern;
        new_result->pattern_index = pattern_index;
        prepend_transition (&new_result->transitions, NULL, result, pool);
      
        rv = pattern_to_nfa_state (pattern->info.star, pattern_index, new_result, pool);
        prepend_transition (&rv->transitions, NULL, new_result, pool);
        prepend_transition (&new_result->transitions, NULL, rv, pool);
        break;
      }
    default:
      dsk_assert_not_reached ();
    }
  return rv;
}

static int
compare_transitions_by_state (const struct NFA_Transition *a,
                              const struct NFA_Transition *b)
{
  return a->next_state < b->next_state ? -1
       : a->next_state > b->next_state ? +1
       : 0;
}

static void
uniq_transitions (struct NFA_State *state, DskMemPool *pool)
{
  struct NFA_Transition *trans;
  /* sort transitions */
#define COMPARE_TRANSITIONS(a,b, rv) rv = compare_transitions_by_state(a,b)
  DSK_STACK_SORT (NFA_STATE_GET_TRANSITION_STACK (state), COMPARE_TRANSITIONS);
#undef COMPARE_TRANSITIONS

  /* remove dups */
  if (state->transitions != NULL)
    for (trans = state->transitions; trans->next_in_state != NULL; )
      {
        if (trans->next_state == trans->next_in_state->next_state)
          {
            if (compare_char_classes (trans->char_class, trans->next_in_state->char_class) != 0)
              {
                /* Compute the union of the two character classes, and
                   merge them if possible */
                struct CharClass *n = dsk_mem_pool_alloc0 (pool, sizeof (struct CharClass));
                char_class_union_inplace (n, trans->char_class);
                char_class_union_inplace (n, trans->next_in_state->char_class);
                trans->char_class = n;
              }

            /* remove next */
            trans->next_in_state = trans->next_in_state->next_in_state;
          }
        else
          trans = trans->next_in_state;
      }
}

typedef void (*NFA_StateCallback) (struct NFA_State *state,
                                   void             *callback_data);


static void
nfa_state_tree_foreach_1 (struct NFA_State *state,
                          NFA_StateCallback callback,
                          void             *callback_data)
{
  struct NFA_Transition *trans;
restart:
  dsk_assert (!state->flag);
  state->flag = DSK_TRUE;
  callback (state, callback_data);
  {
    struct NFA_State *tail = NULL;
    for (trans = state->transitions; trans; trans = trans->next_in_state)
      if (!trans->next_state->flag)
        {
          if (tail == NULL)
            {
              tail = trans->next_state;
              tail->flag = DSK_TRUE;
            }
          else
            nfa_state_tree_foreach_1 (trans->next_state, callback, callback_data);
        }
    if (tail)
      {
        tail->flag = DSK_FALSE;
        state = tail;
        goto restart;
      }
  }
}
static void
clear_nfa_flags (struct NFA_State *state)
{
  struct NFA_Transition *trans;
restart:
  dsk_assert (state->flag);
  state->flag = 0;
  {
    struct NFA_State *tail = NULL;
    for (trans = state->transitions; trans; trans = trans->next_in_state)
      if (trans->next_state->flag)
        {
          if (tail == NULL)
            {
              tail = trans->next_state;
              tail->flag = 0;
            }
          else
            clear_nfa_flags (trans->next_state);
        }
    if (tail)
      {
        tail->flag = 1;
        state = tail;
        goto restart;
      }
  }
}

static void
nfa_state_tree_foreach   (struct NFA_State *state,
                          NFA_StateCallback callback,
                          void             *callback_data)
{
  nfa_state_tree_foreach_1 (state, callback, callback_data);
  clear_nfa_flags (state);
}

/* Remove all transitions with char_class==NULL
   by copying the target's transitions into the current state's list.
   Then recurse. */
static void
nfa_state_prune_null_transitions (struct NFA_State *state,
                                  void *callback_data)
{
  struct NFA_Transition *unhandled_null_transitions = NULL;
  struct NFA_Transition *null_transitions = NULL;
  struct NFA_Transition **ptrans;
  DskMemPool *pool = callback_data;

  /* Split the transition list into two halves -- one that contains 
     CharClass==NULL, and the other that doesn't */
  for (ptrans = &state->transitions; *ptrans != NULL; )
    if ((*ptrans)->char_class == NULL)
      {
        struct NFA_Transition *to_reduce = *ptrans;
        *ptrans = to_reduce->next_in_state;
        to_reduce->next_in_state = unhandled_null_transitions;
        unhandled_null_transitions = to_reduce;
      }
    else
      {
        ptrans = &((*ptrans)->next_in_state);
      }

  /* For each CharClass==NULL transition, add its real transitions 
     our pruned list (dedup) and its NULL transitions to our "to-process" list.
     Use a marker to avoid cycles. */
  while (unhandled_null_transitions)
    {
      struct NFA_Transition *t = *ptrans;
      for (t = null_transitions; t; t = t->next_in_state)
        if (t->next_state == unhandled_null_transitions->next_state)
          break;
      if (t)
        unhandled_null_transitions = unhandled_null_transitions->next_in_state;
      else
        {
          struct NFA_Transition *p = unhandled_null_transitions;
          struct NFA_State *dest = p->next_state;
          unhandled_null_transitions = p->next_in_state;
          p->next_in_state = null_transitions;
          null_transitions = p;

          if (dest->is_match)
            state->is_match = 1;

          /* Copy all ParseState's from dest to
             either unhandled_null_transitions or state's transition list */
          for (p = dest->transitions; p; p = p->next_in_state)
            prepend_transition (p->char_class != NULL
                                    ? &state->transitions
                                    : &unhandled_null_transitions,
                                p->char_class, p->next_state,
                                pool);
        }
    }

  uniq_transitions (state, pool);
}


static void
nfa_prune_null_transitions (struct NFA_State *state,
                            DskMemPool       *pool)
{
  nfa_state_tree_foreach (state, nfa_state_prune_null_transitions, pool);
}

#if DEBUG_NFA
static void
dump_nfa_node (struct NFA_State *state, void *callback_data)
{
  struct NFA_Transition *trans;
  DSK_UNUSED (callback_data);
  if (state->is_match)
    printf ("%p: [match %u]\n", state, state->pattern_index);
  else
    printf ("%p:\n", state);
  for (trans = state->transitions; trans; trans = trans->next_in_state)
    {
      printf ("   ");
      if (trans->char_class == NULL)
        printf ("[empty]");
      else
        print_char_class (trans->char_class);
      printf (" -> %p\n", trans->next_state);
    }
}
static void
dump_nfa (struct NFA_State *state)
{
  nfa_state_tree_foreach (state, dump_nfa_node, NULL);
}
#endif

#if 0
/* --- Deterministic Finite Automata --- */
/* Convert sets of states into single states with transitions 
   mutually exclusive */

struct _Transition
{
  char c;
  State *next;
};

struct _State
{
  unsigned hash;
  unsigned n_transitions;
  Transition *transitions;
  State *default_next;
  State *hash_next;
  State *uninitialized_next;
  unsigned positions[1];
};

struct _StateSlab
{
  StateSlab *next_slab;
};

struct _StateTable
{
  unsigned n_entries;
  unsigned table_size;
  State **hash_table;
  unsigned occupancy;

  unsigned sizeof_slab;
  unsigned sizeof_state;
  unsigned n_states_in_slab;
  unsigned n_states_free_in_slab;
  State *next_free_in_cur_slab;
  StateSlab *cur_slab;

  State *uninitialized_list;
};

typedef enum
{
  REPEAT_EXACT,
  REPEAT_OPTIONAL,
  REPEAT_STAR,
  REPEAT_PLUS
} RepeatType;

struct _ParsedEntry
{
  CharClass *char_class;
  RepeatType repeat_type;
};

/* things are parsed in order, each entry adds N states:
     EXACT:    1 -- is after char
     OPTIONAL: 1 -- is after char
     REPEATED: ...
 */

static void
state_table_init (StateTable *table, unsigned n_entries)
{
  table->n_entries = n_entries;
  table->table_size = 19;
  table->hash_table = dsk_malloc0 (sizeof (State*) * table->table_size);
  table->occupancy = 0;
  table->sizeof_state = DSK_ALIGN (sizeof (State) + (n_entries-1) * sizeof(unsigned), sizeof (void*));
  table->n_states_in_slab = 16;
  table->sizeof_slab = sizeof (StateSlab) + table->sizeof_state * table->n_states_in_slab;
  table->n_states_free_in_slab = 0;
  table->next_free_in_cur_slab = NULL;		/* unneeded */
  table->cur_slab = NULL;
  table->uninitialized_list = NULL;
};

static State *
state_table_force (StateTable *table, const unsigned *positions)
{
  unsigned hash = state_hash (table->n_entries, positions);
  unsigned idx = hash % table->n_entries;
  State *at = table->hash_table[idx];
  while (at != NULL)
    {
      if (memcmp (at->positions, positions, sizeof (unsigned) * table->n_entries) == 0)
        return at;
      at = at->hash_next;
    }

  /* Allocate a state */
  if (table->n_states_free_in_slab == 0)
    {
      StateSlab *slab = dsk_malloc (table->sizeof_slab);
      table->next_free_in_cur_slab = (State *)(slab + 1);
      table->n_states_free_in_slab = table->n_states_in_slab;
      slab->next_slab = table->cur_slab;
      table->cur_slab = slab;
    }
  at = table->next_free_in_cur_slab;
  table->next_free_in_cur_slab = (State *) ((char*)table->next_free_in_cur_slab + table->sizeof_state);
  table->n_states_free_in_slab -= 1;

  table->occupancy++;
  if (table->occupancy * 2 > table->table_size)
    {
      unsigned new_table_size = ...;
      ...
      idx = hash % table->table_size;
    }

  /* Initialize it */
  memcpy (at->positions, positions, sizeof (unsigned) * table->n_entries);
  at->hash = hash;
  at->n_transitions = 0;
  at->transitions = NULL;
  at->default_next = NULL;

  /* add to hash-table */
  at->hash_next = table->hash_table[idx];
  table->hash_table[idx] = at;

  /* add to uninitialized list */
  at->uninitialized_next = table->uninitialized_list;
  table->uninitialized_list = at;

  return at;
}
#endif

struct DFA_State_Range
{
  uint8_t start;
  uint8_t n;
  uint8_t is_constant;
  union {
    struct DFA_State *constant;
    struct DFA_State **nonconstant;
  } info;
};

struct DFA_State
{
  void *match_result;
  unsigned is_match : 1;
  unsigned n_ranges : 31;
  struct DFA_State_Range *ranges;
};

struct DFA_TreeNode
{
  struct DFA_TreeNode *next_skeletal;   /* if skeletal */
  struct DFA_TreeNode *parent, *left, *right;
  dsk_boolean is_red;
  struct DFA_State *state;
  unsigned n_nfa;
  struct NFA_State *states[1];  /* more follow; must be sorted */
};

struct _DskPattern
{
  DskMemPool state_pool;
  struct DFA_State *init_state;
};

#define COMPARE_NFA_STATES_TO_DFA_TREE(_n_nfa, _nfa_states, tree_node, rv) \
  rv = memcmp (_nfa_states, tree_node->states,             \
               sizeof (struct NFA_State *)                     \
               * MIN (_n_nfa, tree_node->n_nfa));       \
  if (rv == 0)                                                 \
    {                                                          \
      if (_n_nfa > tree_node->n_nfa)                    \
        rv = 1;                                                \
      else if (_n_nfa < tree_node->n_nfa)               \
        rv = -1;                                               \
    }
#define COMPARE_TREE_NODES(a,b, rv) \
  COMPARE_NFA_STATES_TO_DFA_TREE(a->n_nfa, a->states, b, rv)
#define GET_NFA_SUBSET_TREE(tree_top) \
  (tree_top), struct DFA_TreeNode *, \
  DSK_STD_GET_IS_RED, DSK_STD_SET_IS_RED, \
  parent, left, right, \
  COMPARE_TREE_NODES

static void
count_nfa_states_one (struct NFA_State *state,
                       void *p_count_inout)
{
  unsigned *count = p_count_inout;
  DSK_UNUSED (state);
  *count += 1;
}
static struct DFA_State *
force_dfa (struct DFA_TreeNode **p_tree,
           DskMemPool           *tree_pool,
           struct DFA_TreeNode **p_skeleton_list,
           DskMemPool           *final_pool,
           unsigned              n_nfa_states,
           struct NFA_State    **nfa_states,
           const DskPatternEntry*entries)
{
  unsigned i, o;
  struct DFA_TreeNode *tree_node;
  dsk_boolean is_match;
  unsigned match_index;
  struct DFA_TreeNode *conflict;

  if (n_nfa_states == 0)
    return NULL;

  /* Sort/unique nfa_states */
#define COMPARE_NFA_STATES(a,b,rv) rv = (a<b) ? -1 : (a>b) ? 1 : 0;
  DSK_QSORT (nfa_states, struct NFA_State *, n_nfa_states, COMPARE_NFA_STATES);
#undef COMPARE_NFA_STATES
  o = 0;
  for (i = 1; i < n_nfa_states; i++)
    if (nfa_states[o] != nfa_states[i])
      nfa_states[++o] = nfa_states[i];
  n_nfa_states = o + 1;

#if IMPLEMENT_TRUMP_STATES
  /* Implement state-trumping */
  //...
#endif


#define COMPARE_NFA_SUBSET(a,b, rv)                            \
  COMPARE_NFA_STATES_TO_DFA_TREE(n_nfa_states, nfa_states, b, rv)
  DSK_RBTREE_LOOKUP_COMPARATOR (GET_NFA_SUBSET_TREE (*p_tree),
                                unused, COMPARE_NFA_SUBSET, tree_node);
  if (tree_node != NULL)
    return tree_node->state;
  tree_node = dsk_mem_pool_alloc (tree_pool,
                                  sizeof (struct DFA_TreeNode)
                                  + sizeof (struct NFA_State *) * (n_nfa_states - 1));
  memcpy (tree_node->states, nfa_states, sizeof (struct NFA_State *) * n_nfa_states);
  tree_node->n_nfa = n_nfa_states;
  tree_node->state = dsk_mem_pool_alloc0 (final_pool, sizeof (struct DFA_State));
  is_match = DSK_FALSE;
  match_index = 0;
  for (i = 0; i < n_nfa_states; i++)
    if (nfa_states[i]->is_match)
      {
        if (!is_match)
          {
            match_index = nfa_states[i]->pattern_index;
            is_match = DSK_TRUE;
          }
        else if (match_index > nfa_states[i]->pattern_index)
          match_index = nfa_states[i]->pattern_index;
      }
  if (is_match)
    {
      tree_node->state->is_match = 1;
      tree_node->state->match_result = entries[match_index].result;
    }

  /* add tree node to tree */
  DSK_RBTREE_INSERT (GET_NFA_SUBSET_TREE (*p_tree), tree_node, conflict);
  dsk_assert (conflict == NULL);

  /* add tree node to skeletal_list */
  tree_node->next_skeletal = *p_skeleton_list;
  *p_skeleton_list = tree_node;

  return tree_node->state;
}

#if DEBUG_DFA
static void
dump_dfa_tree_recursive (struct DFA_TreeNode *node, struct DFA_State *init_dfa)
{
  unsigned i;
  if (node->left)
    dump_dfa_tree_recursive (node->left, init_dfa);

  printf("dfa:%p [nfa:", node->state);
  for (i = 0; i < node->n_nfa; i++)
    printf (" %p", node->states[i]);
  printf ("] (%u ranges)", node->state->n_ranges);
  if (node->state == init_dfa)
    printf (" [INITIAL]");
  printf ("\n");
  for (i = 0; i < node->state->n_ranges; i++)
    {
      struct DFA_State_Range *range = node->state->ranges + i;
      if (range->is_constant)
        if (range->n == 1)
          printf ("  %s -> %p\n", 
                  dsk_ascii_byte_name (range->start),
                  range->info.constant);
        else
          printf ("  %s - %s -> %p\n", 
                  dsk_ascii_byte_name (range->start),
                  dsk_ascii_byte_name (range->start + range->n - 1),
                  range->info.constant);
      else
        {
          unsigned j;
          for (j = 0; j < node->state->ranges[i].n; j++)
            if (range->info.nonconstant[j])
              printf ("  %s -> %p\n",
                    dsk_ascii_byte_name (range->start + j),
                    range->info.nonconstant[j]);
        }
    }

  if (node->right)
    dump_dfa_tree_recursive (node->right, init_dfa);
}
#endif

DskPattern *dsk_pattern_compile (unsigned n_entries,
                                 DskPatternEntry *entries,
                                 DskError **error)
{
  DskMemPool mem_pool = DSK_MEM_POOL_STATIC_INIT;
  unsigned i;
  struct NFA_State **init_states;
  unsigned n_nfa_states;
  struct DFA_TreeNode *tree = NULL;
  DskMemPool final_pool = DSK_MEM_POOL_STATIC_INIT;
  DskMemPool tree_pool = DSK_MEM_POOL_STATIC_INIT;
  struct DFA_State *init_dfa;
  struct DFA_TreeNode *skeletal_list = NULL;
  unsigned nfa_state_pad_size = 16;
  struct NFA_State **nfa_state_pad;
  DskPattern *rv;

  init_states = dsk_mem_pool_alloc (&mem_pool, sizeof (struct NFA_State *) * n_entries);
  for (i = 0; i < n_entries; i++)
    {
      struct Token *tokens;
      struct Pattern *pattern;
      struct NFA_State *final_state, *init_state;
      if (!tokenize (entries[i].pattern, &tokens, &mem_pool, error))
        {
          dsk_add_error_prefix (error, "tokenizing pattern %u", i);
          dsk_mem_pool_clear (&mem_pool);
          return NULL;
        }
#if DEBUG_TOKENIZE
      dsk_warning ("tokens for pattern %u", i);
      dump_token_list (tokens);
#endif
      pattern = parse_pattern (i, tokens, &mem_pool, error);
      if (pattern == NULL)
        {
          dsk_add_error_prefix (error, "parsing pattern %u", i);
          dsk_mem_pool_clear (&mem_pool);
          return NULL;
        }
#if DEBUG_PARSE
      dsk_warning ("tree for pattern %u", i);
      dump_pattern (pattern, 0);
#endif
      final_state = dsk_mem_pool_alloc (&mem_pool, sizeof (struct NFA_State));
      final_state->pattern = NULL;
      final_state->transitions = NULL;
      final_state->is_match = DSK_TRUE;
      final_state->flag = 0;
      final_state->pattern_index = i;
      init_state = pattern_to_nfa_state (pattern, i, final_state, &mem_pool);
      dsk_assert (init_state);
#if DEBUG_NFA
      dsk_warning ("before de-nullification");
      dump_nfa (init_state);
#endif
      nfa_prune_null_transitions (init_state, &mem_pool);
#if DEBUG_NFA
      dsk_warning ("after de-nullification");
      dump_nfa (init_state);
#endif

      /* TO CONSIDER: merge states that are the same (this may lead to further merging) */

      init_states[i] = init_state;
    }

  n_nfa_states = 0;
  for (i = 0; i < n_entries; i++)
    {
      nfa_state_tree_foreach (init_states[i], count_nfa_states_one, &n_nfa_states);
    }

#if IMPLEMENT_TRUMP_STATES
  /* implement state trumping: if two sections are
     isomorphic starting at the final state,
     then any DFA state containing both is equivalent to
     the DFA state containing just one. */
  for (i = 0; i < n_entries; i++)
    nfa_state_tree_foreach (init_states[i], compute_reverse_transitions_1, &mem_pool);

  /* divide states into equivalence classes */
  for (i = 0; i < n_entries; i++)
    if (final_states[i]->trump_class == NULL)
      {
        /* find the smallest subgraph containing final_states[i] with no outlinks */
        ...

        for (j = 1; j < n_entries; j++)
          if (final_states[j]->trump_class == NULL)
            {
              test_subgraph_nodes[0] = final_states[j];
              if (subgraphs_equiv (n_closed_subgraph_nodes, closed_subgraph_nodes,
                                   test_subgraph_nodes))
                {
                  /* make an equiv class for nodes of i (if this is the first match) and for j. */
                  for (k = 0; k < n_closed_subgraph_nodes; k++)
                    {
                      if (closed_subgraph_nodes[k]->equiv_class == NULL)
                        closed_subgraph_nodes[k]->equiv_class = dsk_mem_pool_alloc (&mem_pool, sizeof (TrumpEquivClass));
                      test_subgraph_nodes[k]->equiv_class = closed_subgraph_nodes[k]->equiv_class;
                    }
                }
            }
        }

  /* TODO: try to extend existing equivalent classes */
#endif

  /* OK, now we have the total NFA.
     Convert to DFA by making each node correspond to a subset of
     the NFA nodes in the tree.

     The transitions out of a DFA node are the union of
     the transitions out of all if its NFA nodes.
     However, we group by unique char class.
     The recursive procedure is: 
       If a NFA subset is found in the NFA-subset -> DFA tree,
          return it.
       Otherwise,
          Create the skeletal DFA node.
          Insert skeletal into tree.
          For each character,
             Find the set of NFA states that apply.
             Find the DFA node (recursively - may return skeleton).
          For each unique DFA node, find the set of chars
          that leads to it.
          The transition table is chars -> DFA node; this
          node is no longer skeletal.
      To avoid a huge stack, we don't use actual recursion
      but merely put skeletal DFA nodes onto a worklist.
    */
  //size_t estimated_nfa_size = sizeof (struct DFA_State) * n_nfa_states * 2;
  //void *slab = dsk_malloc (estimated_nfa_size);
  //dsk_mem_pool_init_buf (&regex_pool, estimated_nfa_size, slab);
  dsk_mem_pool_init (&final_pool);

  /* TRUMP: will need to pass in state-trumping info */
  init_dfa = force_dfa (&tree, &tree_pool, /* The tree and its allocator */
                        &skeletal_list,    /* list of skeletal DFA_Nodes */
                        &final_pool,       /* Allocator for DFA_Nodes */
                        n_entries, init_states,
                        entries);

  nfa_state_pad = DSK_NEW_ARRAY (nfa_state_pad_size, struct NFA_State *);

  while (skeletal_list != NULL)
    {

      /* TODO: optimize this for a lot of common cases:
          '.', single-chars, single-case */
      struct DFA_State *char_nodes[256];

      struct DFA_State_Range ranges[256];     /* need fewer, i think */
      unsigned n_ranges;
      unsigned o = 0;

      /* Remove from skeletal_list */
      struct DFA_TreeNode *to_flesh_out = skeletal_list;
      skeletal_list = skeletal_list->next_skeletal;

      for (i = 1; i < 256; i++)
        {
          unsigned n_nfa_states = 0;
          unsigned j;
          struct NFA_Transition *trans;
          for (j = 0; j < to_flesh_out->n_nfa; j++)
            for (trans = to_flesh_out->states[j]->transitions;
                 trans != NULL; trans = trans->next_in_state)
              {
                if (CHAR_CLASS_IS_SET(trans->char_class, i))
                  {
                    if (n_nfa_states == nfa_state_pad_size)
                      {
                        nfa_state_pad_size *= 2;
                        nfa_state_pad = dsk_realloc (nfa_state_pad,
                                                     sizeof (struct NFA_State *) * nfa_state_pad_size);
                      }
                    nfa_state_pad[n_nfa_states++] = trans->next_state;
                  }
              }
          char_nodes[i] = force_dfa (&tree, &tree_pool,
                                     &skeletal_list, &final_pool,
                                     n_nfa_states, nfa_state_pad, entries);
        }


      /* Compress char_nodes into a sequence of ranges */
      n_ranges = 0;
      for (i = 1; i < 256;    )
        if (char_nodes[i] != NULL)
          {
            unsigned n = 1;
            while (n + i < 256 && char_nodes[i]==char_nodes[i+n])
              n++;
            ranges[n_ranges].start = i;
            ranges[n_ranges].n = n;
            ranges[n_ranges].is_constant = 1;
            ranges[n_ranges].info.constant = char_nodes[i];
            n_ranges++;
            i += n;
          }
        else
          i++;
      for (i = 0; i < n_ranges; )
        {
          if (ranges[i].n == 1 && i + 1 < n_ranges && ranges[i+1].n == 1
           && ranges[i+1].start == ranges[i].start + 1)
            {
              struct DFA_State **nonconstant;
              unsigned n = 1;
              unsigned j;
              while (i + n < n_ranges && ranges[i+n].n == 1
                     && ranges[i+n].start == ranges[i].start + n)
                n++;
              
              dsk_assert (n >= 2);
              nonconstant = dsk_mem_pool_alloc (&final_pool, sizeof (struct DFA_State *) * n);
              for (j = 0; j < n; j++)
                nonconstant[j] = ranges[i+j].info.constant;
              ranges[o].is_constant = 0;
              ranges[o].start = ranges[i].start;
              ranges[o].n = n;
              ranges[o].info.nonconstant = nonconstant;
              o++;
              i += n;
            }
          else
            {
              ranges[o++] = ranges[i++];
            }
        }

      /* Deskeletonize */
      to_flesh_out->state->n_ranges = o;
      to_flesh_out->state->ranges = dsk_mem_pool_alloc (&final_pool, sizeof (struct DFA_State_Range) * o);
      memcpy (to_flesh_out->state->ranges, ranges, sizeof (struct DFA_State_Range) * n_ranges);
    }

#if DEBUG_DFA
  dump_dfa_tree_recursive (tree, init_dfa);
#endif

  dsk_mem_pool_clear (&mem_pool);
  dsk_mem_pool_clear (&tree_pool);
  dsk_free (nfa_state_pad);

  rv = DSK_NEW (DskPattern);
  rv->state_pool = final_pool;
  rv->init_state = init_dfa;
  return rv;
}


void *
dsk_pattern_match (DskPattern *pattern,
                   const char *str)
{
  struct DFA_State *state = pattern->init_state;
  if (state == NULL)
    return NULL;
  while (*str)
    {
      /* Looking next state for character */
      unsigned start_range = 0, range_count = state->n_ranges;
      uint8_t c = *str;
      struct DFA_State *next_state;
      while (range_count > 0)
        {
          unsigned mid_range = start_range + range_count / 2;
          struct DFA_State_Range *R = &state->ranges[mid_range];
          if (c < R->start)
            range_count /= 2;
          else if (c >= R->start + R->n)
            {
              range_count -= range_count / 2 + 1;
              start_range = mid_range + 1;
            }
          else if (R->is_constant)
            {
              next_state = R->info.constant;
              goto found_next_state;
            }
          else
            {
              next_state = R->info.nonconstant[c - R->start];
              if (next_state == NULL)
                return NULL;
              goto found_next_state;
            }
        }
      return NULL;

found_next_state:
      state = next_state;
      str++;
    }
  if (!state->is_match)
    return NULL;
  return state->match_result;
}

void
dsk_pattern_free (DskPattern *pattern)
{
  dsk_mem_pool_clear (&pattern->state_pool);
  dsk_free (pattern);
}
