{
    "variables": {
        "dummy": "<!(./clean.sh && ./build-gyp.sh 2>&1 && ./copy.sh)",
    },
    "targets": [{
        "target_name": "dummy"
    }]
}