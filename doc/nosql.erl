% A Node.

-type node_status() :: uninitialized
                     | syncing
		     | ready
		     .

-record(node_config, {
        }).
-record(config, {
         node_list :: [node_config()]
       }).
-record(state, {
         current_status :: node_status (),
	 active_transfers :: [transfer ()]
       }).
-record(client_query, {
         table :: binary (),
	 key :: binary (),
	 n_r :: integer (),           % # of responses to wait for
       }).
-record(client_update, {
         table :: binary (),
	 key :: binary (),
	 subkey = <<>> :: binary()    % for hash-type tables
       }).


run () ->
  receive 
    {cluster_info_changed, Nodes} ->
      ...;

    {bucket_transfer, Bucket, Pid} ->
      ...;

    {client_query, Pid, Queries} ->
      ...;

    {client_update, Pid, Updates} ->
      ...
  after
    ...
  end.


