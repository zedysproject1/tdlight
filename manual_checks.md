# Manual checks
## After every merge
1. Find if any `get_channel_difference` has been added to `MessagesManager.cpp`, and replace it with `delayed_get_channel_difference`.

    ### Example
    The following line
    
    ```cpp
    get_channel_difference(dialog_id, old_pts, true, "add_pending_channel_update pts mismatch");
     ```
    
    should become
    
    ```cpp
    auto enable_reactive_channel_difference
      = G()->shared_config()
        .get_option_boolean("enable_reactive_channel_difference", false);

    get_channel_difference_delayed(dialog_id, old_pts, true,
      enable_reactive_channel_difference,
      "add_pending_channel_update pts mismatch");
    ```

2. Find `make_unique<Dialog>` and check if there's `d.set_time()` after.