Module['preRun'].push(function() {
    addRunDependency('fsync');

    FS.mkdir('/storage');
    FS.mount(IDBFS, {autoPersist: true}, '/storage');

    FS.syncfs(true, function(err) {
        if (err) throw err;
        removeRunDependency('fsync');
        console.log("Filesystem synced successfully.");
    });
})
