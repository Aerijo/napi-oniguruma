{
    "targets": [
        {
            "target_name": "oniguruma_scanner",
            "dependencies": [
                "oniguruma",
            ],
            "sources": [
                "src/oniguruma.c",
            ],
        },
        {
            "target_name": "oniguruma",
            "type": "static_library",
            "direct_dependent_settings": {
                "include_dirs": [
                    "third_party/oniguruma/src",
                ],
            },
            "include_dirs": [
                "third_party/oniguruma",
                "third_party/oniguruma/src",
            ],
            "conditions": [
                ["OS==\"win\"", {
                    "defines": [
                        "ONIG_EXTERN=extern", # TODO: Check if required
                    ],
                    "direct_dependent_settings": {
                        "defines": [
                            "ONIG_EXTERN=extern",
                        ],
                    },
                    "include_dirs": [
                        "third_party/oniguruma",
                    ],
                    "sources": [
                        "third_party/oniguruma/src/unicode_fold1_key.c",
                        "third_party/oniguruma/src/unicode_fold2_key.c",
                        "third_party/oniguruma/src/unicode_fold3_key.c",
                        "third_party/oniguruma/src/unicode_unfold_key.c",
                    ]
                }],
                ["OS==\"linux\"", {
                    "cflags": [
                        "-w",
                    ],
                }],
                ["OS==\"mac\"", {
                    "cflags": [
                        "-w",
                    ],
                }],
            ],
            "sources": [
                "third_party/oniguruma/src/regcomp.c",
                "third_party/oniguruma/src/regenc.c",
                "third_party/oniguruma/src/regerror.c",
                "third_party/oniguruma/src/regext.c",
                "third_party/oniguruma/src/regexec.c",
                "third_party/oniguruma/src/regparse.c",
                "third_party/oniguruma/src/regsyntax.c",
                "third_party/oniguruma/src/regtrav.c",
                "third_party/oniguruma/src/regversion.c",
                "third_party/oniguruma/src/st.c",
                "third_party/oniguruma/src/reggnu.c",
                "third_party/oniguruma/src/regposerr.c",
                "third_party/oniguruma/src/regposix.c",
                "third_party/oniguruma/src/mktable.c",
                "third_party/oniguruma/src/ascii.c",
                "third_party/oniguruma/src/euc_jp.c",
                "third_party/oniguruma/src/euc_tw.c",
                "third_party/oniguruma/src/euc_kr.c",
                "third_party/oniguruma/src/sjis.c",
                "third_party/oniguruma/src/big5.c",
                "third_party/oniguruma/src/gb18030.c",
                "third_party/oniguruma/src/koi8.c",
                "third_party/oniguruma/src/koi8_r.c",
                "third_party/oniguruma/src/cp1251.c",
                "third_party/oniguruma/src/iso8859_1.c",
                "third_party/oniguruma/src/iso8859_2.c",
                "third_party/oniguruma/src/iso8859_3.c",
                "third_party/oniguruma/src/iso8859_4.c",
                "third_party/oniguruma/src/iso8859_5.c",
                "third_party/oniguruma/src/iso8859_6.c",
                "third_party/oniguruma/src/iso8859_7.c",
                "third_party/oniguruma/src/iso8859_8.c",
                "third_party/oniguruma/src/iso8859_9.c",
                "third_party/oniguruma/src/iso8859_10.c",
                "third_party/oniguruma/src/iso8859_11.c",
                "third_party/oniguruma/src/iso8859_13.c",
                "third_party/oniguruma/src/iso8859_14.c",
                "third_party/oniguruma/src/iso8859_15.c",
                "third_party/oniguruma/src/iso8859_16.c",
                "third_party/oniguruma/src/utf8.c",
                "third_party/oniguruma/src/utf16_be.c",
                "third_party/oniguruma/src/utf16_le.c",
                "third_party/oniguruma/src/utf32_be.c",
                "third_party/oniguruma/src/utf32_le.c",
                "third_party/oniguruma/src/unicode.c",
            ],
        }
    ]
}
