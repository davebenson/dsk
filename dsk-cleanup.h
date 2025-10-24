
/* can be used to free stuff before program termination */
/* not normally needed but when analyzing programs
 * for memory leaks, this can be helpful
 * in reducing noise.  */
void dsk_cleanup (void);
