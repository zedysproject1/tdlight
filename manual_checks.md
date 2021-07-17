# Manual checks
## After every merge
1.  Find if any `get_channel_difference` has been added to `MessagesManager.cpp`, and replace it with `delayed_get_channel_difference`.

    ### Example
    The following line
    
    ```cpp
    get_channel_difference(dialog_id, old_pts, true, "add_pending_channel_update pts mismatch");
     ```
    
    should become
    
    ```cpp
    auto enable_pull_based_backpressure
      = G()->shared_config()
        .get_option_boolean("enable_pull_based_backpressure", false);

    get_channel_difference_delayed(dialog_id, old_pts, true,
      enable_pull_based_backpressure,
      "add_pending_channel_update pts mismatch");
    ```
