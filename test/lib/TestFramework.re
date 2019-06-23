let current_dir = Sys.getcwd();

include Rely.Make({
  let config =
    Rely.TestFrameworkConfig.initialize({
      snapshotDir: current_dir ++ "/test/lib/__snapshots__",
      projectDir: current_dir,
    });
});
