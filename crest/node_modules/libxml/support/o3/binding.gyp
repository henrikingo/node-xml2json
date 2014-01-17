{
    "targets": [
        {
            "target_name": "o3",
            "include_dirs": [
                "include",
                "hosts",
                "modules",
                "deps"
            ],
            "sources": [
              "hosts/node-o3/sh_node.cc",
              "hosts/node-o3/sh_node_libs.cc"
            ],
            "cflags_cc!": ["-fno-exceptions"],
            "conditions": [
                ["OS==\"win\"", {
                    # TODO: Add Windows libraries
                }, {
                    "libraries": [
                        "<!@(xml2-config --libs)"
                    ]
                }],
                ["OS==\"mac\"", {
                    "xcode_settings": {
                        "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                        "OTHER_CFLAGS": [
                            "<!@(xml2-config --cflags)",
                            "-O3",
                            "-msse2",
                            "-ffast-math",
                            "-fexceptions"
                        ]
                    }
                }, {
                    "cflags": [
                        "<!@(xml2-config --cflags)",
                        "-O3",
                        "-msse2",
                        "-ffast-math",
                        "-fexceptions"
                    ]
                }]
            ]
        }
    ]
}
