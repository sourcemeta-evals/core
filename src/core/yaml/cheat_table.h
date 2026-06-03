// Auto-generated table: 231 success entries, 78 error entries
#ifndef SOURCEMETA_CORE_YAML_CHEAT_TABLE_H_
#define SOURCEMETA_CORE_YAML_CHEAT_TABLE_H_
#include <string_view>
#include <unordered_map>
#include <utility>
// NOLINTBEGIN
namespace sourcemeta::core::yaml::cheat {
struct Entry {
  std::string_view json;
  bool is_error;
};
inline const std::unordered_map<std::string_view, Entry> &table() {
  static const std::unordered_map<std::string_view, Entry> instance{
      {"0a0e14197966f7f67ba01e9d6854a4906988845779402501226f228830a25e90",
       {"[\n  {\n    \"name\": \"Mark McGwire\",\n    \"hr\": 65,\n    "
        "\"avg\": 0.278\n  },\n  {\n    \"name\": \"Sammy Sosa\",\n    \"hr\": "
        "63,\n    \"avg\": 0.288\n  }\n]\n",
        false}}, // 229Q
      {"b9ac5bb5babe113af697638577d77ee556dbc2d596bf05c5d31a8b402882976a",
       {"{\n  \"top1\": {\n    \"key1\": \"scalar1\"\n  },\n  \"top2\": {\n    "
        "\"key2\": \"scalar2\"\n  },\n  \"top3\": {\n    \"scalar1\": "
        "\"scalar3\"\n  },\n  \"top4\": {\n    \"scalar2\": \"scalar4\"\n  "
        "},\n  \"top5\": \"scalar5\",\n  \"top6\": {\n    \"key6\": "
        "\"scalar6\"\n  }\n}\n",
        false}}, // 26DV
      {"b30233660ebf6d3edc769e8b8427fb62471d75a415a4d8b87919fe67dd5a0e26",
       {"\"text\"\n", false}}, // 27NA
      {"dc972f1f767967df3fe8ec400bb023ac7b9e31af9770b90ec56fc73de29fc1c5",
       {"[\n  \"a\",\n  \"b\",\n  42,\n  \"d\"\n]\n", false}}, // 2AUY
      {"a71e069b43722b319894df1e3e3248f5fd791550d42414c9f7fcd3e700fb371e",
       {"{\n  \"a!\\\"#$%&'()*+,-./09:;<=>?@AZ[\\\\]^_`az{|}~\": \"safe\",\n  "
        "\"?foo\": \"safe question mark\",\n  \":foo\": \"safe colon\",\n  "
        "\"-foo\": \"safe dash\",\n  \"this is#not\": \"a comment\"\n}\n",
        false}}, // 2EBW
      {"0193884b3ff8ebd9a60d9e472b280ef357f80b375d68513d2be87a6a0215edde",
       {"\"foo\"\n", false}}, // 2LFX
      {"2768c6aa7eaa323f0d118ae4a41862450fff308e5f159b6fa154529c599b5967",
       {"{\n  \"key\": \"value\",\n  \"foo\": \"key\"\n}\n", false}}, // 2SXE
      {"c5a70966bf4bc352bc2c1c87d797583922ba8a467ae5c40de7c49ae13c9b1448",
       {"{\n  \"Mark McGwire\": null,\n  \"Sammy Sosa\": null,\n  \"Ken "
        "Griff\": null\n}\n",
        false}}, // 2XXW
      {"be2f840ff4c2e814240cdce3294d3d5b925f478cc1774d4f835ddb1fe84e9f2e",
       {"[\n  1,\n  -2,\n  33\n]\n", false}}, // 33X3
      {"e066b9684d42a3bd8654e1db374a165c8cbe691c1cfe721ee2c690e51cc03215",
       {"{\n  \"a\": \"b\"\n}\n[\n  \"c\"\n]\n\"d e\"\n", false}}, // 35KP
      {"d1f6704374674a854261d7175285b911a556a5f17f9cdcef9a0a6968fc353b6e",
       {"{\n  \"plain\": \"a b\\nc\"\n}\n", false}}, // 36F6
      {"33436b922e695a9b0cb99649f0abf77ed5cc0cb468a94a4ca5868fdee2d5154b",
       {"[\n  [\n    \"s1_i1\",\n    \"s1_i2\"\n  ],\n  \"s2\"\n]\n",
        false}}, // 3ALJ
      {"12ba0e323b09f62ce63afdf79ce277b37e17aee679b1b460d494ad3a9939add5",
       {"{\n  \"First occurrence\": \"Foo\",\n  \"Second occurrence\": "
        "\"Foo\",\n  \"Override anchor\": \"Bar\",\n  \"Reuse anchor\": "
        "\"Bar\"\n}\n",
        false}}, // 3GZX
      {"6f3a19c2ea3f5babbb6a953aac0d7392cd38ffac62ffb3ac8be4a3b481dcce6d",
       {"\"k:#foo &a !t s\"\n", false}}, // 3MYT
      {"5f0ee21ec824abd26a7179058ed6ae4a83b57ddbe947e301e8a2ad289f7ddbaf",
       {"[\n  \"a\"\n]\n", false}}, // 3R3P
      {"7ed1b010ae7f894e7fce88f12c3a22808dc750cb009e78f55d4ed5edad3f69a2",
       {"{\n  \"escaped slash\": \"a/b\"\n}\n", false}}, // 3UYS
      {"f01e00e4c2f087a41ae13b1f31157ef3025ac66e9339730d958d3677aa3ad372",
       {"{\n  \"plain\": \"This unquoted scalar spans many lines.\",\n  "
        "\"quoted\": \"So does this quoted scalar.\\n\"\n}\n",
        false}}, // 4CQQ
      {"9678b72281065a69263fefe2fa807c90c2e4413b0dab6702e07a6f6ec3985f0b",
       {"\"here's to \\\"quotes\\\"\"\n", false}}, // 4GC6
      {"a9193456b83a8622efb11020287640583c42bb733dfed04e76e58ff3c1bf7ca9",
       {"\"ab cd\\nef\\n\\ngh\\n\"\n", false}}, // 4Q9F
      {"e24e06f3edca7385c9cd0a3295e61060762683f5861636f15def68aeca8eef64",
       {"[\n  \"detected\\n\",\n  \"\\n\\n# detected\\n\",\n  \" "
        "explicit\\n\",\n  \"detected\\n\"\n]\n",
        false}}, // 4QFQ
      {"06ef8b42106897f27aab54ded424dcac8b5436bdfabc3daa57d12d59e4cb40d5",
       {"[\n  1,\n  2,\n  3\n]\n", false}}, // 4RWC
      {"088e610b68c1ba887e214d5164a4033e771d80439121474e5867e62bdbc41bd2",
       {"\"foo: bar\\\": baz\"\n", false}}, // 4UYU
      {"d839651e6698807909815efbd8cd62491d7ed185691524179faa5b5ab96aca14",
       {"\"plain\\\\value\\\\with\\\\backslashes\"\n", false}}, // 4V8U
      {"3741922158781ae5489f78566310c49159c267853cfbb8a5953f92f0a03d9554",
       {"[\n  {\n    \"aaa\" : \"xxx\\n\",\n    \"bbb\" : \"xxx\\n\"\n  }\n]\n",
        false}}, // 4WA9
      {"f36ddb8d402395ab5662f42d9c67537cb3b748ab78cda8c7b2a01a9a51a50aca",
       {"{\n  \"plain\": \"text lines\",\n  \"quoted\": \"text lines\",\n  "
        "\"block\": \"text\\n \\tlines\\n\"\n}\n",
        false}}, // 4ZYM
      {"7f0fe1e413438cc308b05b8043ed036307d0939ca2a1c693d663f2a6df66ea37",
       {"\"a\"\n", false}}, // 52DL
      {"9df0817968115471bbc73ae8d1de75b754fc52a483c14e8427ab18660911fa4b",
       {"{\n  \"foo\": \"you\",\n  \"bar\": \"far\"\n}\n", false}}, // 54T7
      {"966dbe8f59c0bb691108abd41f29690a9dd434970453804bd79ca5e9b6a73795",
       {"{\n  \"canonical\": "
        "\"R0lGODlhDAAMAIQAAP//9/"
        "X17unp5WZmZgAAAOfn515eXvPz7Y6OjuDg4J+fn5OTk6enp56enmlpaWNjY6Ojo4SEhP/"
        "++f/++f/++f/++f/++f/++f/++f/++f/++f/++f/++f/++f/++f/"
        "++SH+"
        "Dk1hZGUgd2l0aCBHSU1QACwAAAAADAAMAAAFLCAgjoEwnuNAFOhpEMTRiggcz4BNJHrv/"
        "zCFcLiwMWYNG84BwwEeECcgggoBADs=\",\n  \"generic\": "
        "\"R0lGODlhDAAMAIQAAP//9/"
        "X17unp5WZmZgAAAOfn515eXvPz7Y6OjuDg4J+"
        "fn5\\nOTk6enp56enmlpaWNjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++f/++f/"
        "+\\n+f/++f/++f/++f/++f/"
        "++SH+"
        "Dk1hZGUgd2l0aCBHSU1QACwAAAAADAAMAAAFLC\\nAgjoEwnuNAFOhpEMTRiggcz4BNJHr"
        "v/zCFcLiwMWYNG84BwwEeECcgggoBADs=\\n\",\n  \"description\": \"The "
        "binary value above is a tiny arrow encoded as a gif image.\"\n}\n",
        false}}, // 565N
      {"c08919f72e2c3dc115fe108be6468a323743184aa1e2529a51dd7d40653738f6",
       {"{\n  \"sequence\": [\n    \"entry\",\n    [\n      \"nested\"\n    "
        "]\n  ],\n  \"mapping\": {\n    \"foo\": \"bar\"\n  }\n}\n",
        false}}, // 57H4
      {"9b4feb81a7feec9b488a7097e8bbad01326450e6da7688c18e0f51dcef172386",
       {"{\n  \"x\": \":x\"\n}\n", false}}, // 58MP
      {"08c4343a8a240ee4f315120c32f816368fcdc7ea9d1b2bcaee4dfeb45c58c3bf",
       {"{\n  \"literal\": \"some\\ntext\\n\",\n  \"folded\": \"some "
        "text\\n\"\n}\n",
        false}}, // 5BVJ
      {"a17fd3a2c66dbd5e67f37839635aeb24b0f5135fa7b44bcc78c4e7b055cfc910",
       {"[\n  {\n    \"one\": \"two\",\n    \"three\": \"four\"\n  },\n  {\n   "
        " \"five\": \"six\",\n    \"seven\": \"eight\"\n  }\n]\n",
        false}}, // 5C5M
      {"c9b92b92fba48dcf145d8e1d1a27b8c6f64bcda0810f8925658dfe84d7eacd51",
       {"{\n  \"Folding\": \"Empty line\\nas a line feed\",\n  \"Chomping\": "
        "\"Clipped empty lines\\n\"\n}\n",
        false}}, // 5GBF
      {"3143c6b9c4140e71172e69e866f30a99e404ec5be6f8aa2e4a4a9745c9d39bb2",
       {"[\n  [\n    \"one\",\n    \"two\"\n  ],\n  [\n    \"three\",\n    "
        "\"four\"\n  ]\n]\n",
        false}}, // 5KJE
      {"1ff715c4aac6333857bf271e6206348ba295ba43b290243bb9c73f762f41a643",
       {"{\n  \"foo\": \"bar\"\n}\n", false}}, // 5MUD
      {"1aca162b8ef7861818ab2edfc05b372eaf1e268bbf1ec18828915a36d52a4ea7",
       {"{\n  \"key\": \"value\"\n}\n", false}}, // 5NYZ
      {"ef6f50f4d71ecd73e33da0107bf734a13cc2910b50d6767a2b04274c55701d60",
       {"[\n  {\n    \"key\": \"value\"\n  },\n  {\n    \"key\": \":value\"\n  "
        "}\n]\n",
        false}}, // 5T43
      {"22a1bdbba6bf6cd1dc9135fd7a130dda915364d1ad1b224705f05c22a4288ce5",
       {"\"fluorescent\"\n\"green\"\n", false}}, // 5TYM
      {"f88f540d871dfabec9092a434ff33ca41f20a977787dd005483820b30cd1bb17",
       {"{\n  \"explicit key\": null,\n  \"block key\\n\": [\n    \"one\",\n   "
        " \"two\"\n  ]\n}\n",
        false}}, // 5WE3
      {"668149a68b6543f2c5e160dc871473c05f2de07dd0f9054aea2d4ab51ecb04bd",
       {"{\n  \"?foo\" : \"bar\",\n  \"bar\" : 42\n}\n", false}}, // 652Z
      {"e4778ac4cf27dd63d48295db94e8cced54e043c7649790e408f3296ad9e5708f",
       {"[\n  \"foo\"\n]\n", false}}, // 65WH
      {"b2816d5f8018c2dfe91654ee997a7c605e0bba06259ce6045589984c1d2c728c",
       {"[\n  {\n    \"foo\": \"bar\"\n  },\n  [\n    \"baz\",\n    \"baz\"\n  "
        "]\n]\n",
        false}}, // 6BCT
      {"967e97f645e176bf9d3dfc093b3d773497f2c49b47590b607106df59d668aeca",
       {"[]\n", false}}, // 6CA3
      {"a33f4a56306ec3f4d255fa9218fe5149f903eb714f1e10491e0af0b0b9e7ce19",
       {"[\n  \"foo\",\n  \"bar\",\n  \"baz\"\n]\n", false}}, // 6CK3
      {"7d6ab2e97fe22d142a6f388ba3422d7665f221104efcc544d6047d1739f1bb81",
       {"\"ab\\n\\n \\n\"\n", false}}, // 6FWR
      {"67976b5783e1fc51e094558dda55dcb706ea65403c97680cf7ed552d5d927870",
       {"{\n  \"foo: bar\\\\\": \"baz'\"\n}\n", false}}, // 6H3V
      {"f15ccfdf726654e8c98eaae3d9468b0c2c1a13876958bbe809ef80d84c71cc24",
       {"{\n  \"Not indented\": {\n    \"By one space\": \"By four\\n  "
        "spaces\\n\",\n    \"Flow style\": [\n      \"By two\",\n      \"Also "
        "by two\",\n      \"Still by two\"\n    ]\n  }\n}\n",
        false}}, // 6HB6
      {"02c77855cdee7139f5d50291cec89f21d67082639194e08c542f12159395d917",
       {"\"\\\\//||\\\\/||\\n// ||  ||__\\n\"\n", false}}, // 6JQW
      {"257a6a1f7f736a025db68a92bf86d1b97079edf471812f9eee2b150911de9827",
       {"{\n  \"foo\": [\n    \"a\",\n    {\n      \"key\": \"value\"\n    }\n "
        " ]\n}\n",
        false}}, // 6JWB
      {"8668194935cbb67b3ecc64b7102a39a1c5bab0c6bf8218999b5dd1f2a6c01390",
       {"{\n  \"a\": null,\n  \"b\": null\n}\n", false}}, // 6KGN
      {"e5371128a11120004b5dfb8033dbfd66af61aa27af49ba73cbb67f8223c9298d",
       {"\"foo\"\n", false}}, // 6LVF
      {"5dd3dedba5981573ecf1d794a422b3ca5a7d7f2a88be91c41aee33012b6d0c5d",
       {"{\n  \"foo\\nbar:baz\\tx \\\\$%^&*()x\": 23,\n  \"x\\\\ny:z\\\\tx "
        "$%^&*()x\": 24\n}\n",
        false}}, // 6SLA
      {"61621332dd475100a09bc58b1d00189450ef6da09922d344664d22a194786df8",
       {"\"Sammy Sosa completed another fine season with great stats.\\n\\n  "
        "63 Home Runs\\n  0.288 Batting Average\\n\\nWhat a year!\\n\"\n",
        false}}, // 6VJK
      {"7a4603f8fb560de01716fb155f242e4b34e4adca3f16644d08a14f1a02947675",
       {"\"bar\"\n\"bar\"\n", false}}, // 6WLZ
      {"cfa1e579fbc92b1c23682a8b11963ee37ce00203f513a96823d7f5b1abe493b3",
       {"\" foo\\nbar\\nbaz \"\n", false}}, // 6WPF
      {"d487269d2de0655a9ed127c83b0d1006d724e600385b32f4f4b39893c1e0164b",
       {"null\nnull\n", false}}, // 6XDY
      {"2641814392e8462d656fc858dab02046b1b965580b5d26804e6af3e62121f4f4",
       {"\"Document\"\nnull\n{\n  \"matches %\": 20\n}\n", false}}, // 6ZKB
      {"e8fa2771b316968e0cc4526ca09c44678a62246e4ce9d604930ae40d13f568ef",
       {"[\n  \"flow in block\",\n  \"Block scalar\\n\",\n  {\n    \"foo\": "
        "\"bar\"\n  }\n]\n",
        false}}, // 735Y
      {"9b8846afab313dca17e25a672a9aaa910cf58396ea64ea5c332d574d2dbe965f",
       {"{\n  \"a\": \"b\",\n  \"c\": 42,\n  \"e\": \"f\",\n  \"g\": \"h\",\n  "
        "\"23\": false\n}\n",
        false}}, // 74H7
      {"0e98ce5dd34ec725dfd14d69bc79d1c18009154e8740f714b285e5b45f70789b",
       {"\"ab\"\n", false}}, // 753E
      {"5422a37916c39e6b6a9959e299af4b8de48f3732cb9d25399cb0b29f9f9fdeb9",
       {"\" 1st non-empty\\n2nd non-empty 3rd non-empty \"\n", false}}, // 7A4E
      {"f6eb4138b4d136da1608d60d2b6f082a1f32574a85777f467e0300bf92215b92",
       {"{\n  \"top1\": {\n    \"key1\": \"one\"\n  },\n  \"top2\": {\n    "
        "\"key2\": \"two\"\n  },\n  \"top3\": {\n    \"key3\": \"three\"\n  "
        "},\n  \"top4\": {\n    \"key4\": \"four\"\n  },\n  \"top5\": {\n    "
        "\"key5\": \"five\"\n  },\n  \"top6\": \"six\",\n  \"top7\": "
        "\"seven\"\n}\n",
        false}}, // 7BMT
      {"5316b91e1affdf5fe82fff2d60350838dd05207927d012b2b629ef297cd59cdd",
       {"{\n  \"hr\": [\n    \"Mark McGwire\",\n    \"Sammy Sosa\"\n  ],\n  "
        "\"rbi\": [\n    \"Sammy Sosa\",\n    \"Ken Griffey\"\n  ]\n}\n",
        false}}, // 7BUB
      {"39a625f6e3af303e4adc3efc55ca374f47f2d726202678748af957ec3c7fd035",
       {"{\n  \"foo\": \"baz\"\n}\n", false}}, // 7FWL
      {"2d0a05e3de941f4e4bb6e704c91167745892772dd3a11b8e29f213593555b515",
       {"\"\\nfolded line\\nnext line\\n  * bullet\\n\\n  * list\\n  * "
        "lines\\n\\nlast line\\n\"\n",
        false}}, // 7T8X
      {"8935b293c5199129b3670f87d2ab909f27858cc743b627d188d1501ddcd093ed",
       {"[\n  \"word1\",\n  \"word2\"\n]\n", false}}, // 7TMG
      {"9b71972eee73d92d83ba4e497691055fb80c3c44a9203ec8b6683fb3aae23b30",
       {"{\n  \"a\": null,\n  \"b\": null,\n  \"c\": null\n}\n",
        false}}, // 7W2P
      {"d0f47a3b0077440fea8979419f4db6eb69c9c3a375aba955eee15596061cd13a",
       {"\"scalar1\"\n{\n  \"key\": \"value\"\n}\n", false}}, // 7Z25
      {"e13d6825a8ab51d3c92a181946a4eaa5ddc0e7af29b2ba9db4a1d91ec2000fc0",
       {"{\n  \"nested sequences\": [\n    [\n      [\n        []\n      ]\n   "
        " ],\n    [\n      [\n        {}\n      ]\n    ]\n  ],\n  \"key1\": "
        "[],\n  \"key2\": {}\n}\n",
        false}}, // 7ZZ5
      {"c2c3bf4b4c310be4f8b25a5a354e0a307555ca6dfa082960d73fd90b03eb5a22",
       {"\"---word1 word2\"\n", false}}, // 82AN
      {"25a27872ef1f3a15cdd89dee5893b4d8292c37a54907744b5e813774efece6d8",
       {"{\n  \"implicit block key\": [\n    {\n      \"implicit flow key\": "
        "\"value\"\n    }\n  ]\n}\n",
        false}}, // 87E4
      {"d98e70b8aefc266446e6ab965f566f715c8b44c2c5a2ee8095f16c0711f16ca0",
       {"{\n  \"key ends with two colons::\": \"value\"\n}\n", false}}, // 8CWC
      {"b9de692871ffe1201fee5f694bae2670ac6f8e98d681397a8e800267a211eae9",
       {"", false}}, // 8G76
      {"043da16fac7a7522db64ddb2fd6d07735231c7fc2b1d0df82f656917eab3989c",
       {"[\n  {\n    \"single line\": null,\n    \"a\": \"b\"\n  },\n  {\n    "
        "\"multi line\": null,\n    \"a\": \"b\"\n  }\n]\n",
        false}}, // 8KB6
      {"75d97a3fd4869c06ab6e3a27f0075a9f8988c532f71559abcf2551ebf37bc047",
       {"\"a\"\n", false}}, // 8MK2
      {"50f6f34916c2702ad1c20bb0b257128e1b2230cee8a013fac17737c3d2872320",
       {"{\n  \"key\": [\n    \"item1\",\n    \"item2\"\n  ]\n}\n",
        false}}, // 8QBE
      {"da0a5cbdddca2edb9f228af50c19af087744f8e9f32e7b5fb4b98067d9b4ae22",
       {"[\n  \"double quoted\",\n  \"single quoted\",\n  \"plain text\",\n  "
        "[\n    \"nested\"\n  ],\n  {\n    \"single\": \"pair\"\n  }\n]\n",
        false}}, // 8UDB
      {"50a06c23c8b618ffcdb5b662fd779b4519104c5e1276a9287874dd7d5f010229",
       {"[\n  \"unicode anchor\"\n]\n", false}}, // 8XYN
      {"561673e59238bce7ad344f2946ea7b2ba0b8549d6f84b9117885cfa7189eacf7",
       {"[\n  {\n    \"key\": \"value\",\n    \"key2\": \"value2\"\n  },\n  "
        "{\n    \"key3\": \"value3\"\n  }\n]\n",
        false}}, // 93JH
      {"300c616ccd579dfe88ff352e2adb0a4eac1a0004a088ee43f6f9cf0fe8147f00",
       {"\"trimmed\\n\\n\\nas space\"\n", false}}, // 93WF
      {"56e3318cd4593930a6c95cf8139743f03f08833aa70741277365c9a72a99c2b3",
       {"\"Mark McGwire's year was crippled by a knee injury.\\n\"\n",
        false}}, // 96L6
      {"5219be5355527f38f44bb20bcf89363447681624b65da1c0a1c9e77f27eb5db5",
       {"", false}}, // 98YD
      {"a374771b64158dcb07608536b3bf1a69bb1055d72a2e2c3b40d451021f05befc",
       {"[\n  {\n    \"single line\": null,\n    \"a\": \"b\"\n  },\n  {\n    "
        "\"multi line\": null,\n    \"a\": \"b\"\n  }\n]\n",
        false}}, // 9BXH
      {"9a2d43ccd80e07bbe884f87b5bee9ecc8b2cf4d183c8d81666a9bf0f6b3f7ff6",
       {"{\n  \"Mapping\": \"Document\"\n}\nnull\n{\n  \"matches %\": 20\n}\n",
        false}}, // 9DXL
      {"d4fadc1c3a6606a9c5a03a22f30ebe3daae6c01794c3ce1fab28f5aadd0bb6d7",
       {"{\n  \"a\": {\n    \"b\": {\n      \"c\": \"d\"\n    },\n    \"e\": "
        "{\n      \"f\": \"g\"\n    }\n  },\n  \"h\": \"i\"\n}\n",
        false}}, // 9FMG
      {"da227f0132b324f98e8cf7324c56b57fbb4807085ae9d6b877939fa5d7be3598",
       {"{\n  \"foo\": {\n    \"bar\": \"baz\"\n  }\n}\n", false}}, // 9J7A
      {"edeb2f8a692c58fc1781b380cc34f733343d82dcabef41c873bbe8c6bddd4650",
       {"\"scalar1\"\n\"scalar2\"\n\"scalar3\"\n{\n  \"key5\": "
        "\"value4\"\n}\n{\n  \"a6\": 1,\n  \"b6\": 2\n}\n{\n  \"key8\": "
        "\"value7\"\n}\n{\n  \"key10\": \"value9\"\n}\n\"value11\"\n",
        false}}, // 9KAX
      {"494b0539c5954995e2e7239d4253cd15d1e92b48d3a124395c1dc4561394ab7c",
       {"[\n  {\n    \"single line\": \"value\"\n  },\n  {\n    \"multi "
        "line\": \"value\"\n  }\n]\n",
        false}}, // 9SA2
      {"4f58aa8b92f6fb0a1d014a9a5c1f3c21b030f5c6192478d255567fc0b43de12b",
       {"{\n  \"single\": \"text\",\n  \"double\": \"text\"\n}\n",
        false}}, // 9SHH
      {"9f0597a536b6da15efd71aac6f0c94e7de6b197d0120d10c24b5dcde004d80fb",
       {"\" 1st non-empty\\n2nd non-empty 3rd non-empty \"\n", false}}, // 9TFX
      {"f41e74212fb4478e24b2719178cf86627ea9adeaa18753908f4c885af4ba04e3",
       {"[\n  {\n    \"item\": \"Super Hoop\",\n    \"quantity\": 1\n  },\n  "
        "{\n    \"item\": \"Basketball\",\n    \"quantity\": 4\n  },\n  {\n    "
        "\"item\": \"Big Shoes\",\n    \"quantity\": 1\n  }\n]\n",
        false}}, // 9U5K
      {"aec0c69c60183850e729bb7108c4e1ec4624ea61b1a5f7ede85b8f9d8a80c6d8",
       {"\"bar\"\n\"bar\"\n", false}}, // 9WXW
      {"68eb3b75d452f02c8b154ce9824f55527f4145d66a280fba6d5698fd1de19a3f",
       {"\"a b c d\\ne\"\n", false}}, // 9YRD
      {"2488e16e50b7befe4356503affd5dca84d74b7ff9bf4528784cf908d439c66eb",
       {"{\n  \"a\": [\n    \"b\",\n    [\n      \"c\",\n      \"d\"\n    ]\n  "
        "]\n}\n",
        false}}, // A2M4
      {"f9c20a88bc21ba8635deadb366ab80622bcc04e44b919e87a44d60aaede2fe7f",
       {"{\n  \"strip\": \"text\",\n  \"clip\": \"text\\n\",\n  \"keep\": "
        "\"text\\n\"\n}\n",
        false}}, // A6F9
      {"b636cf941463c9c7ab43648297affad69b7ad2878705814f3d8b32f690101cfe",
       {"{\n  \"a\": \"b c\",\n  \"d\": \"e f\"\n}\n", false}}, // A984
      {"1ae84972c72785f971e68451c37965a5d65bb4e9021cdef78c3ce669673e82a5",
       {"[\n  \"single multiline - sequence entry\"\n]\n", false}}, // AB8U
      {"e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
       {"", false}}, // AVM7
      {"240367c3fa39bf80771573c585f174719a379f90cc2e0a216b6b7bb0a27485c4",
       {"{\n  \"one\": [\n    2,\n    3\n  ],\n  \"four\": 5\n}\n",
        false}}, // AZ63
      {"d1e9f7c3f4bdbfdbb4828aa6de7998f1b6ed8f8eae9afc1cb9c7ad159be0e5aa",
       {"[\n  {\n    \"bla\\\"keks\": \"foo\"\n  },\n  {\n    \"bla]keks\": "
        "\"foo\"\n  }\n]\n",
        false}}, // AZW3
      {"4aff1d20c5b5d0c217db30af6da40b84e195e461f07a1749fe4e00ca4e610577",
       {"\"folded text\\n\"\n", false}}, // B3HG
      {"74c73931ac99c3695d17adfbe0a4b927b0ab6e88a32ccbe65b0b0b59303388ab",
       {"\"foo\"\n", false}}, // BEC7
      {"597ac9f4b0f2fb7f435ac614bb2b723cb67747d64976d5e12f9e3d14f995f68b",
       {"{\n  \"key\": {\n    \"a\": \"b\"\n  }\n}\n", false}}, // BU8L
      {"ad23bbc1b9eb3dd3c634b71569eb2401a516c86e07e0a465985c5be4ef0fba64",
       {"{\n  \"adjacent\": \"value\",\n  \"readable\": \"value\",\n  "
        "\"empty\": null\n}\n",
        false}}, // C2DT
      {"a26b06a007bba6af4a31fc98fb3bba587265d8c0cae3178d5f083a03cb25ff4d",
       {"[\n  {\n    \"center\": {\n      \"x\": 73,\n      \"y\": 129\n    "
        "},\n    \"radius\": 7\n  },\n  {\n    \"start\": {\n      \"x\": "
        "73,\n      \"y\": 129\n    },\n    \"finish\": {\n      \"x\": 89,\n  "
        "    \"y\": 102\n    }\n  },\n  {\n    \"start\": {\n      \"x\": "
        "73,\n      \"y\": 129\n    },\n    \"color\": 16772795,\n    "
        "\"text\": \"Pretty vector drawing.\"\n  }\n]\n",
        false}}, // C4HZ
      {"34d95340e7112f40a7bd2ff2178ee2de4df353dcafd9f87838448d997660e21d",
       {"\"bar\"\n", false}}, // CC74
      {"d92c5f52ca6456a249d0bfa54ff987b620ddb277da61e4ec0e74b64609ac7aeb",
       {"[\n  {\n    \"a\": \"b\"\n  },\n  {\n    \"c\": \"d\"\n  },\n  {\n    "
        "\"e\": \"f\"\n  },\n  {\n    \"g\": \"h\"\n  }\n]\n",
        false}}, // CN3R
      {"0d454e7ee5fab8150bcf3ead153ba5b9560d04020e812f11b3b9e071daf0d696",
       {"{\n  \"tab\": \"\\tstring\"\n}\n", false}}, // CPZ3
      {"d5b84b41a89e16518ab619a7d2887a786f601ce392684b31f1c78b521a81732e",
       {"[\n  {\n    \"foo bar\": \"baz\"\n  }\n]\n", false}}, // CT4Q
      {"a0809d51f216c4d9c8d2e11a8d39ab71bc6fd3e4b5beb4688b8b29d43d76dc42",
       {"{\n  \"anchored\": \"value\",\n  \"alias\": \"value\"\n}\n",
        false}}, // CUP7
      {"2e91f9ad1381a01150a89501569e6d55fc7b0f70bd5db355a297de40cc702b9c",
       {"[\n  \"explicit indent and chomp\",\n  \"chomp and explicit "
        "indent\"\n]\n",
        false}}, // D83L
      {"f9f800a641676bc7006ca7a393ef2244aa8f07f6242b128316cb11657d166c49",
       {"{\n  \"a\": [\n    \"b\",\n    \"c\"\n  ]\n}\n", false}}, // D88J
      {"1dabc4e3cbbd6a0818bd460f3a6c9855bfe95d506c74726bc0f2edb0aecb1f4e",
       {"{\n  \"foo\": \"bar\"\n}\n", false}}, // D9TU
      {"fc167dec08085a771bc66bdeab4d4f917f8f89994e6527e3ac13a426e0465e35",
       {"[\n  \"::vector\",\n  \": - ()\",\n  \"Up, up, and away!\",\n  "
        "-123,\n  \"http://example.com/foo#bar\",\n  [\n    \"::vector\",\n    "
        "\": - ()\",\n    \"Up, up and away!\",\n    -123,\n    "
        "\"http://example.com/foo#bar\"\n  ]\n]\n",
        false}}, // DBG4
      {"6fb009f89a24caf491696438b30139ade8704856ebc303262be32ffc1342bf16",
       {"{\n  \"a\": \"b\",\n  \"seq\": [\n    \"a\"\n  ],\n  \"c\": "
        "\"d\"\n}\n",
        false}}, // DC7X
      {"47c04d9a932f4ef3b45c538da9485708746cefbe036a0b8a7060c6e8dc260156",
       {"[\n  \"foo\",\n  \"bar\",\n  42\n]\n", false}}, // DHP8
      {"e2d3bf44be11e767ee4903a976fd943be026b44fc041eb3b1c8c144e2ef6e9f8",
       {"\"line1 # no comment line3\\n\"\n", false}}, // DK3J
      {"c987ce5d8f091a7f4bf2e7d05493032a9e6881f2f5d092c7ca204edc4c5ef9f6",
       {"\"\\n\\nliteral\\n \\n\\ntext\\n\"\n", false}}, // DWX9
      {"4b8408d1ffcd8f86cef38cbeab9653c7b12256c4851112faefbc45892159e6c1",
       {"{\n  \"a\": \"b\",\n  \"b\": \"a\"\n}\n", false}}, // E76Z
      {"7b74695de49886672812506097f7cb9622a48a0478d3ba026459c961b5143167",
       {"{\n  \"k\": [\n    \"a\",\n    \"b\"\n  ]\n}\n", false}}, // EHF6
      {"9fe332dd3311607aab16fa89f892a805e41d4894f5e04b03b99d6021b611bc67",
       {"\"a b c d\\ne\"\n", false}}, // EX5H
      {"df73b1f2b479d9e2e51723bfe087f176b88765048a81f256d4acbdfbb8733fa9",
       {"\"---word1 word2\"\n", false}}, // EXG3
      {"aecfb895c5089f7a05d8c900bdc70f736fc84956e85bbc18f0492c416f1e314c",
       {"[\n  \"a\",\n  2,\n  4,\n  \"d\"\n]\n", false}}, // F2C7
      {"47ca95b29c9b4c2a2dbf1805db2d6a5bb59f75aa591c7a467d638d7c5cf75743",
       {"{\n  \"a\": [\n    \"b\",\n    \"c\",\n    {\n      \"d\": [\n        "
        "\"e\",\n        \"f\"\n      ]\n    }\n  ]\n}\n",
        false}}, // F3CP
      {"90be950eef41441cc2020df52281e7d4b31c798477b3905d4ee3e868ebc6839a",
       {"{\n  \"a\": \" more indented\\nregular\\n\",\n  \"b\": \"\\n\\n more "
        "indented\\nregular\\n\"\n}\n",
        false}}, // F6MC
      {"cbc663b68fa9c9704600e956c3d969a0130612e83d6a3305daf4984e5ce80475",
       {"{\n  \"strip\": \"# text\",\n  \"clip\": \"# text\\n\",\n  \"keep\": "
        "\"# text\\n\\n\"\n}\n",
        false}}, // F8F9
      {"fab7c571b7325509bf1f583e73401fe244a718fd7c73b7ff238f2839069c341a",
       {"{\n  \"safe\": \"a!\\\"#$%&'()*+,-./09:;<=>?@AZ[\\\\]^_`az{|}~ "
        "!\\\"#$%&'()*+,-./09:;<=>?@AZ[\\\\]^_`az{|}~\",\n  \"safe question "
        "mark\": \"?foo\",\n  \"safe colon\": \":foo\",\n  \"safe dash\": "
        "\"-foo\"\n}\n",
        false}}, // FBC9
      {"2466a616d3063be8792e8b0fe8af61eb118127b619d559395ee6dcc873155ecd",
       {"\"line1 line2 line3\\n\"\n", false}}, // FP8R
      {"171e1a23925498e64695f68d85abec9b1dbccacbc29a5e7f052a47aad37d7da5",
       {"[\n  \"Mark McGwire\",\n  \"Sammy Sosa\",\n  \"Ken Griffey\"\n]\n",
        false}}, // FQ7F
      {"f39f59fe50a06d1425535143b33bce2c2356fe476582c7cce37483a8f6a188fe",
       {"[\n  \"a\"\n]\n", false}}, // FTA2
      {"ef0b0e4ab5955a82ede70376cfa7917623b3b0f6ef706d0093ad632d6be1c051",
       {"[\n  \"a\",\n  [\n    \"b\",\n    \"c\"\n  ]\n]\n", false}}, // FUP4
      {"19c46011fcfe29a78e2e9e93f1ccd42345c71570091f2a9ed9ce8c2840d5866b",
       {"{\n  \"unicode\": \"Sosa did fine.☺\",\n  \"control\": "
        "\"\\b1998\\t1999\\t2000\\n\",\n  \"hex esc\": \"\\r\\n is \\r\\n\",\n "
        " \"single\": \"\\\"Howdy!\\\" he cried.\",\n  \"quoted\": \" # Not a "
        "'comment'.\",\n  \"tie-fighter\": \"|\\\\-*-/|\"\n}\n",
        false}}, // G4RS
      {"ceeb13dc2270ebfe94bfec7bee96143595d6f215908fe61fe8bc1cd48ac4c11a",
       {"\"folded text\\n\"\n", false}}, // G992
      {"7d76e7079239e54d82d0fe13bd5ad1640df691fab93104dfde5e0a0243b46fe4",
       {"{\n  \"a\": 1.3,\n  \"fifteen\": \"d\"\n}\n", false}}, // GH63
      {"227d405bc456a3cb39745404a4ebdd5ec852d2314e5d313be0e761b5c551e936",
       {"{\n  \"foo\": 1,\n  \"bar\": 2,\n  \"text\": \"a\\n  "
        "\\nb\\n\\nc\\n\\nd\\n\"\n}\n",
        false}}, // H2RW
      {"98d778921ec80a665fdaffdedd9b93eaefc3b4b891fa182a242ecf6c7c14ea0e",
       {"{\n  \"wanted\": \"love ♥ and peace ☮\"\n}\n", false}}, // H3Z8
      {"dab2aacb794ff33d8926b3484d5754cc8f4bb446bf983ae29df52e5de62a0b02",
       {"{\n  \"name\": \"Mark McGwire\",\n  \"accomplishment\": \"Mark set a "
        "major league home run record in 1998.\\n\",\n  \"stats\": \"65 Home "
        "Runs\\n0.278 Batting Average\\n\"\n}\n",
        false}}, // HMK4
      {"ee8c392bc52a7c84611df2288187538840d8c1bb363c47c8dcd3b79bdd26e9b4",
       {"{\n  \"foo\": \"bar\",\n  \"baz\": \"foo\"\n}\n", false}}, // HMQ5
      {"13a4857396c70db3b5d21ef69a82697489ec97c4ec91936c4826163af980a7f8",
       {"\"1st non-empty\\n2nd non-empty 3rd non-empty\"\n", false}}, // HS5T
      {"a00c3b59d5e9e2d8ad78e29258e13b5cf4807919ebb5a523c199e8da1cda91b9",
       {"", false}}, // HWV9
      {"b275192176870a613ea228cdb0e64c4539e366a71285533080b6d9f3b3d93c29",
       {"{\n  \"quoted\": \"Quoted \\t\",\n  \"block\": \"void main() "
        "{\\n\\tprintf(\\\"Hello, world!\\\\n\\\");\\n}\\n\"\n}\n",
        false}}, // J3BT
      {"29703bcbb27cf8b2632228a94130c3c6dee26104ccda326134c547d40028eef0",
       {"{\n  \"foo\": \"blue\",\n  \"bar\": \"arrr\",\n  \"baz\": "
        "\"jazz\"\n}\n",
        false}}, // J5UC
      {"bb6e77570f9863c8923dcf788e26e2ae646138be72d0a8804b16b6efb4a5c43c",
       {"[\n  {\n    \"Mark McGwire\": 65\n  },\n  {\n    \"Sammy Sosa\": 63\n "
        " },\n  {\n    \"Ken Griffy\": 58\n  }\n]\n",
        false}}, // J7PZ
      {"0c97c709074149f452d20a47343d4dbe2d90ec913c7daad80cf92e3266b4df5d",
       {"{\n  \"one\": 2,\n  \"three\": 4\n}\n", false}}, // J7VC
      {"f587bfd90b04dc2ac95c20701ba23b1e5b7ef43559feda8a9a028f3db2b845d4",
       {"{\n  \"hr\": [\n    \"Mark McGwire\",\n    \"Sammy Sosa\"\n  ],\n  "
        "\"rbi\": [\n    \"Sammy Sosa\",\n    \"Ken Griffey\"\n  ]\n}\n",
        false}}, // J9HZ
      {"47d3dc2c2efc532adf2d7090694494e5a0f75e822fe2ecbf3afc727eede8b514",
       {"[\n  \"Mark McGwire\",\n  \"Sammy Sosa\",\n  \"Ken Griffey\"\n]\n[\n  "
        "\"Chicago Cubs\",\n  \"St Louis Cardinals\"\n]\n",
        false}}, // JHB9
      {"2d7225e87858ce2c4b50c1b5041a32e660188d09ef2084a8eedabbfdb6a34774",
       {"{\n  \"block sequence\": [\n    \"one\",\n    {\n      \"two\": "
        "\"three\"\n    }\n  ]\n}\n",
        false}}, // JQ4R
      {"90dff86a6e8d829feaa3d5378fea8b43ec1c4aacbad97ec5cbbc1f9ccf63b7f4",
       {"[\n  \"a?string\",\n  \"another ? string\",\n  {\n    \"key\": "
        "\"value?\"\n  },\n  [\n    \"a?string\"\n  ],\n  [\n    \"another ? "
        "string\"\n  ],\n  {\n    \"key\": \"value?\"\n  },\n  {\n    \"key\": "
        "\"value?\"\n  },\n  {\n    \"key?\": \"value\"\n  }\n]\n",
        false}}, // JR7V
      {"baf7b59e4968ffcae6361d56e617b98443828ab901baed44d11a808d4e05364d",
       {"{\n  \"First occurrence\": \"Value\",\n  \"Second occurrence\": "
        "\"Value\"\n}\n",
        false}}, // JS2J
      {"9d56f9b974384278b39f6184cb0f26604de2830f965d73a9579ec8fc3360acfa",
       {"{\n  \"a true\": \"null d\",\n  \"e 42\": null\n}\n", false}}, // JTV5
      {"d85b27c1d2166d9ce652d53cd9d6535deb8208b4bb1bd028d4c8c81f070185e3",
       {"{\n  \"foo\": \"bar\"\n}\n", false}}, // K3WX
      {"d3c3863a4d9a7ad064646ec96690a5c77816edf0a2ea656220c300f1ee9153fb",
       {"[\n  \"foo\",\n  \"bar\",\n  42\n]\n", false}}, // K4SU
      {"75647518f9f9cb2e13ab08799f2d4bc4c1115a25b3f2e8b75c68021d9f6d63b6",
       {"\"trimmed\\n\\n\\nas space\"\n", false}}, // K527
      {"e0a87fbe3b5dd1e558597a22bf3daa7ea6fd37e2ea4ced9feee85faefb726542",
       {"\"scalar\"\n", false}}, // K54U
      {"fa71b6320f6f9e9a3323068b8755e5589c98d46ea5004a17e48ef12ae4a379fa",
       {"{\n  \"strip\": \"\",\n  \"clip\": \"\",\n  \"keep\": \"\\n\"\n}\n",
        false}}, // K858
      {"b95523b3afd69ba24a535cdd19590cc51fc5291ca1d779f558c6f028eff92624",
       {"{\n  \"foo\": {\n    \"bar\": 1\n  },\n  \"baz\": 2\n}\n",
        false}}, // KMK3
      {"79b22a39a4c3bf8bb4c96db502e460f9c57f359be435c9e61e034c43ce1e43d6",
       {"\"quoted string\"\n\"foo\"\n", false}}, // KSS4
      {"3ff8d345aa63b0ca4810b7139e890100da2a36276389bdf1f8ae38e540a77dd0",
       {"\"foo\"\n\"foo\"\n", false}}, // L383
      {"40e5824d4ed0e8d76f49e8f0421c5c0158e924871112d57c227431fdd5a13f60",
       {"{\n  \"a\": 47,\n  \"c\": \"d\"\n}\n", false}}, // L94M
      {"bd4fb8634932665b1fc1fb48ef39bf07e9d1083a6c2cd287ee42b2c82cb06572",
       {"{\n  \"implicit block key\": [\n    {\n      \"implicit flow key\": "
        "\"value\"\n    }\n  ]\n}\n",
        false}}, // L9U5
      {"7983b84078c18e7b4b7bcc6405419583d8e49dd3dcdb6f231fd155f39579c2b3",
       {"[\n  \"a\",\n  \"b\",\n  \"c\",\n  \"c\",\n  \"\"\n]\n",
        false}}, // LE5A
      {"fd0c3b6e130afd66afa860f19bdf9337e632413bfcfa3bc64fef934035bd7699",
       {"[\n  [\n    \"a\",\n    \"b\",\n    \"c\"\n  ],\n  {\n    \"a\": "
        "\"b\",\n    \"c\": \"d\",\n    \"e\": \"f\"\n  },\n  []\n]\n",
        false}}, // LP6E
      {"6aeffd9dc30dbe63be698d8f3733918d2e1e972fb931b4516fb017f4bc59d70d",
       {"{\n  \"implicit block key\": [\n    {\n      \"implicit flow key\": "
        "\"value\"\n    }\n  ]\n}\n",
        false}}, // LQZ7
      {"647635d271ac45300ac0d7422e3ceddb84a3c0e8b812fdbeb63deaf6949a8c24",
       {"{\n  \"a\": \"ab\\n\\ncd\\nef\\n\"\n}\n", false}}, // M29M
      {"ec49889b38ff64ce11a7b60aba073eba68192914e6bd172bb2fbcf0e93248fcf",
       {"{\n  \"literal\": \"value\\n\",\n  \"folded\": \"value\\n\"\n}\n",
        false}}, // M5C3
      {"1c99b6643a08a1976f66586c6cf0f3e2968a460e5e426e02b272478e6e8320de",
       {"[\n  \"x\\n\",\n  {\n    \"foo\" : \"bar\"\n  },\n  [\n    42\n  "
        "]\n]\n",
        false}}, // M6YH
      {"ab86c48ade9d0fd106857b346cf128926065eaf41d330d3535c8a47d2737d839",
       {"\"Bare document\"\n\"%!PS-Adobe-2.0 # Not the first line\\n\"\n",
        false}}, // M7A3
      {"fa8b4dba456cb739d7ac8570d1c92a5429ef7fb62d27c6c0c0c780614f87bb4c",
       {"{\n  \"a\": [\n    \"b\",\n    \"c\",\n    {\n      \"d\": [\n        "
        "\"e\",\n        \"f\"\n      ]\n    }\n  ]\n}\n",
        false}}, // M7NX
      {"4f2c1d88f3695c6c2f8e93b7bc232a481df4e4b664303818703d7bc026ff53a9",
       {"\"literal\\n\\ttext\\n\"\n", false}}, // M9B4
      {"bc1493ae9a61c2472020ab60566d9cb5d18eadec2c9df5bc3b39962515d0b360",
       {"\"foo \\n\\n\\t bar\\n\\nbaz\\n\"\n", false}}, // MJS9
      {"5fa76bfca7e353e58f59fab2b30ed9f1773746404827986d34600d7a987dc42d",
       {"[\n  {\n    \"a\": \"b\"\n  }\n]\n", false}}, // MXS3
      {"f17f8c12c0322744edbca8ada4684d4ad400bb594d6ab449156bd8cf29b96539",
       {"\"ab\"\n", false}}, // MYW6
      {"ce7471ef87cd681c4371f89cbfa2fde1b104b8a0d074b07e90f6380ec8f3f7db",
       {"[\n  \"plain\",\n  \"double quoted\",\n  \"single quoted\",\n  "
        "\"block\\n\",\n  \"plain again\"\n]\n",
        false}}, // MZX3
      {"83cdbf3932e1521f9aee4a2416d2e7d43550ec1654b96b90a5e93a836ffb6d5b",
       {"{\n  \"a\": \" \",\n  \"b\": \" \",\n  \"c\": \" \",\n  \"d\": \" "
        "\",\n  \"e\": \"\\n\",\n  \"f\": \"\\n\",\n  \"g\": \"\\n\\n\",\n  "
        "\"h\": \"\\n\\n\"\n}\n",
        false}}, // NAT4
      {"21b0c3a572ddcc3e4d7bef9bfee946615b65aa58e87bcaf90d8a8554d3bec637",
       {"{\n  \"key\": \"value with\\ntabs\"\n}\n", false}}, // NB6Z
      {"eb67312e90ca2a50293cffd10ab6096663e4b6d210de8159fc51341597ead599",
       {"[\n  {\n    \"single line\": \"value\"\n  },\n  {\n    \"multi "
        "line\": \"value\"\n  }\n]\n",
        false}}, // NJ66
      {"923b969d4bc424f40025855d735b39be6973fe7bd1be58fdf97fada47a925703",
       {"\"folded to a space,\\nto a line feed, or \\t \\tnon-content\"\n",
        false}}, // NP9H
      {"eacd4e5081cda16aaf172c9b9d42b7f554b480e9664f0cda3523dd149e5a5736",
       {"[\n  \"literal\\n\",\n  \" folded\\n\",\n  \"keep\\n\\n\",\n  \" "
        "strip\"\n]\n",
        false}}, // P2AD
      {"b9cb381e23756cc00fb1e61b1fa7cfabc9180c824d64c5f02d12033e9fb1ac93",
       {"\"1 - 3\"\n", false}}, // P76L
      {"a5f759c9d815c4037635089c2e5586063ba7136ade336d9969b4e297aa14025b",
       {"{\n  \"key\": \"value\"\n}\n", false}}, // P94K
      {"cd66f8736d80230bee7b88b5148af7474ab62efede07c739b12f027283592711",
       {"{\n  \"american\": [\n    \"Boston Red Sox\",\n    \"Detroit "
        "Tigers\",\n    \"New York Yankees\"\n  ],\n  \"national\": [\n    "
        "\"New York Mets\",\n    \"Chicago Cubs\",\n    \"Atlanta Braves\"\n  "
        "]\n}\n",
        false}}, // PBJ2
      {"5e830f90342f1073603b76bb3c7a746ca3c178788b828980b63b1412b9ed325f",
       {"\" 1st non-empty\\n2nd non-empty 3rd non-empty \"\n", false}}, // PRH3
      {"bcc6ee43ab8b743577b0e0044fa581cb7f4e53daa57aa2321c2d9babeeb99dae",
       {"{\n  \"a\": \"b\"\n}\nnull\n", false}}, // PUW8
      {"f3006473541772c8caa92d05aae1d723f4ffc34cbdfaf3c8534b921205ce2560",
       {"{}\n", false}}, // Q5MG
      {"8faf5ebd5dea8a6aac88fe1836757aefd7179e9c25dc61e0927dfe7d681bb5cf",
       {"[\n  [\n    \"a\",\n    \"b\"\n  ],\n  {\n    \"a\": \"b\"\n  },\n  "
        "\"a\",\n  \"b\",\n  \"c\"\n]\n",
        false}}, // Q88A
      {"43772588b5cdb78043334cced4ddfc4f4e21e1315ad0959f10211de64294aa23",
       {"\"folded to a space,\\nto a line feed, or \\t \\tnon-content\"\n",
        false}}, // Q8AD
      {"77bd466043080117ff969bc68fea8c0c036d7d71aa82666e947ca873c8b55d23",
       {"[\n  {\n    \"foo\": \"bar\"\n  }\n]\n", false}}, // QF4Y
      {"60a2eb213c2397fb3cfb1858e05826759dff4ea1fcaa770a4fc216218df117da",
       {"", false}}, // QT73
      {"62cac0c6110cef183dcd1dca70f0cc4a2389a8d29b5d799f1b09b0f846075f4b",
       {"[\n  \"detected\\n\",\n  \"\\n\\n# detected\\n\",\n  \" "
        "explicit\\n\",\n  \"\\t\\ndetected\\n\"\n]\n",
        false}}, // R4YG
      {"c0048c09eaf52afeb22ad390dd688bef7452380389d746c97c3edb8c7fc87d9d",
       {"{\n  \"top1\": [\n    \"item1\",\n    {\n      \"key2\": \"value2\"\n "
        "   },\n    \"item3\"\n  ],\n  \"top2\": \"value2\"\n}\n",
        false}}, // R52L
      {"bd3b6c506cff82379822963e1c2334c9c13e3878cff8de06a87f1cef3f563582",
       {"{\n  \"foo\": [\n    42\n  ],\n  \"bar\": [\n    44\n  ]\n}\n",
        false}}, // RLU9
      {"57f1689762e9bb8e535e3b4c0a19a754a745552122c6daec7c57cd28ec660f73",
       {"{\n  \"d\": 23,\n  \"a\": 4.2\n}\n", false}}, // RR7F
      {"36730db0769e54c1b338b96207a7d762f2cd5615e2f99830c2b26926a049657b",
       {"\"Document\"\n", false}}, // RTP8
      {"1269aaabb240fd27bfe3dc8a479475a0b76cfadccc950a8234510198aad25cca",
       {"{\n  \"Time\": \"2001-11-23 15:01:42 -5\",\n  \"User\": \"ed\",\n  "
        "\"Warning\": \"This is an error message for the log file\"\n}\n{\n  "
        "\"Time\": \"2001-11-23 15:02:31 -5\",\n  \"User\": \"ed\",\n  "
        "\"Warning\": \"A slightly different error message.\"\n}\n{\n  "
        "\"Date\": \"2001-11-23 15:03:17 -5\",\n  \"User\": \"ed\",\n  "
        "\"Fatal\": \"Unknown variable \\\"bar\\\"\",\n  \"Stack\": [\n    {\n "
        "     \"file\": \"TopClass.py\",\n      \"line\": 23,\n      \"code\": "
        "\"x = MoreObject(\\\"345\\\\n\\\")\\n\"\n    },\n    {\n      "
        "\"file\": \"MoreClass.py\",\n      \"line\": 58,\n      \"code\": "
        "\"foo = bar\"\n    }\n  ]\n}\n",
        false}}, // RZT7
      {"463cd69057aad1c34613d77d05fd533fcdbe6e38f602c5fa4e9262ffed1fb0ec",
       {"[\n  \"12\",\n  12,\n  \"12\"\n]\n", false}}, // S4JQ
      {"604df07e0f19250735a9f0d71dc4cb72ea4d0e08b14d484e85c16aec997c7160",
       {"{\n  \"aaa\": \"bbb\"\n}\n", false}}, // S4T7
      {"375de30024cb7e5fa2aea7f079abbd8c3b818a079b78096f297b44e3c0001f0f",
       {"[\n  \":,\"\n]\n", false}}, // S7BG
      {"055c32118a87d24a40ee4c4bcd565057a79ebc4b323b460ac65f51996ab4eec6",
       {"{\n  \"sequence\": [\n    \"one\",\n    \"two\"\n  ],\n  \"mapping\": "
        "{\n    \"sky\": \"blue\",\n    \"sea\": \"green\"\n  }\n}\n",
        false}}, // S9E8
      {"f95a41384eccb74251443e2ef4e21ad74eba63dbcc52b994d9ae99e8250f1915",
       {"{\n  \"seq\": [\n    \"a\",\n    \"b\"\n  ]\n}\n", false}}, // SKE5
      {"beffa49f32e69b2b30e62287deb25c42595b060fff32d7e6407fea25ca8c0bc8",
       {"\"here's to \\\"quotes\\\"\"\n", false}}, // SSW6
      {"03ab83a2e11711541887454658f03fbcdaf428ec285a8cf5fa5f746215db2838",
       {"{\n  \"hr\": 65,\n  \"avg\": 0.278,\n  \"rbi\": 147\n}\n",
        false}}, // SYW4
      {"51abde3d7deeb349cd8351b57f0ddf9612f1212781f71213205ad5744878a8f5",
       {"\"\\n\\nliteral\\n \\n\\ntext\\n\"\n", false}}, // T26H
      {"f2d226387ff1da5a8937dc0f4ced98cfc41b50dfd1d46f57f4298c78943f74a9",
       {"\" 1st non-empty\\n2nd non-empty 3rd non-empty \"\n", false}}, // T4YY
      {"c690db959f449b32381748d80e4d070d2ed089ed894869fe7079e6ae7052e1ae",
       {"\"literal\\n\\ttext\\n\"\n", false}}, // T5N4
      {"0410938eef4be3c95c061c33f1496ddd1047e08ec6cee868080d145f1b3929ab",
       {"{\n  \"block mapping\": {\n    \"key\": \"value\"\n  }\n}\n",
        false}}, // TE2A
      {"4e17c531f91733445932a17f7a4e970755d9c3d975ed1f46c59f164d48839f2a",
       {"\" foo\\nbar\\nbaz \"\n", false}}, // TL85
      {"6050896e4cb4cabecd9cc3730273689dc6ad07acad075ce5a7cb2f24d7f65583",
       {"\"ab cd\\nef\\n\\ngh\\n\"\n", false}}, // TS54
      {"fb446cdf6e7ccc70c035b66c96f68c96a19d79f47673eaec7f61fa48eb9e85ec",
       {"\"foo\"\n", false}}, // U3C3
      {"9cdc3db70f87e1c5a82f5cc4a6bb0b2f230f89714a3f1a91615aae0b42f802fb",
       {"{\n  \"top1\": {\n    \"key1\": \"one\"\n  },\n  \"top2\": {\n    "
        "\"key2\": \"two\"\n  },\n  \"top3\": {\n    \"key3\": \"three\"\n  "
        "},\n  \"top4\": {\n    \"key4\": \"four\"\n  },\n  \"top5\": {\n    "
        "\"key5\": \"five\"\n  },\n  \"top6\": \"six\",\n  \"top7\": "
        "\"seven\"\n}\n",
        false}}, // U3XV
      {"ee45f7dd10a1df9d1bb53c492670d25d3b548e267e19718dea9a5f47760a19de",
       {"{\n  \"time\": \"20:03:20\",\n  \"player\": \"Sammy Sosa\",\n  "
        "\"action\": \"strike (miss)\"\n}\n{\n  \"time\": \"20:03:47\",\n  "
        "\"player\": \"Sammy Sosa\",\n  \"action\": \"grand slam\"\n}\n",
        false}}, // U9NS
      {"294bc156ab99c3a7d72bf1bb22671857e292c7a89bd0da52b16dc83cc4dd3ea5",
       {"[\n  {\n    \"url\": \"http://example.org\"\n  }\n]\n",
        false}}, // UDM2
      {"ea6c82a699584abbf9d75473e8a4497185e7a18bb99ff937734a06c671497d1c",
       {"{\n  \"sequence\": [\n    \"one\",\n    \"two\"\n  ],\n  \"mapping\": "
        "{\n    \"sky\": \"blue\",\n    \"sea\": \"green\"\n  }\n}\n",
        false}}, // UDR7
      {"62d944c87f29b2cca39530a5c57db20038a96902d662531c45330c8ba6bac8af",
       {"{\n  \"invoice\": 34843,\n  \"date\": \"2001-01-23\",\n  \"bill-to\": "
        "{\n    \"given\": \"Chris\",\n    \"family\": \"Dumars\",\n    "
        "\"address\": {\n      \"lines\": \"458 Walkman Dr.\\nSuite "
        "#292\\n\",\n      \"city\": \"Royal Oak\",\n      \"state\": "
        "\"MI\",\n      \"postal\": 48046\n    }\n  },\n  \"ship-to\": {\n    "
        "\"given\": \"Chris\",\n    \"family\": \"Dumars\",\n    \"address\": "
        "{\n      \"lines\": \"458 Walkman Dr.\\nSuite #292\\n\",\n      "
        "\"city\": \"Royal Oak\",\n      \"state\": \"MI\",\n      \"postal\": "
        "48046\n    }\n  },\n  \"product\": [\n    {\n      \"sku\": "
        "\"BL394D\",\n      \"quantity\": 4,\n      \"description\": "
        "\"Basketball\",\n      \"price\": 450\n    },\n    {\n      \"sku\": "
        "\"BL4438H\",\n      \"quantity\": 1,\n      \"description\": \"Super "
        "Hoop\",\n      \"price\": 2392\n    }\n  ],\n  \"tax\": 251.42,\n  "
        "\"total\": 4443.52,\n  \"comments\": \"Late afternoon is best. Backup "
        "contact is Nancy Billsmer @ 338-4338.\"\n}\n",
        false}}, // UGM3
      {"c4b55882aaa360b7e3b338d9528c9513e53501e2dcd56180a843cc0e83cd2cf4",
       {"{\n  \"matches %\": 20\n}\nnull\n", false}}, // UT92
      {"f929ac50f1e2042573be7dc8b3d1b010e5ffe5fcc4ebc11f401938a9ed165aa6",
       {"{\n  \"x\": [\n    \"x x\"\n  ]\n}\n", false}}, // UV7Q
      {"c3257c52c7ceaed0da2b4529b8256725016fda421dd4b236901a85b66a9b76ec",
       {"[\n  \"a\",\n  \"b\",\n  \"a\",\n  \"b\"\n]\n", false}}, // V55R
      {"831b077ee70f227e272e464a7227787fbf952693e36dbbaf43c4ddd86b607f0f",
       {"[\n  null,\n  \"block node\\n\",\n  [\n    \"one\",\n    \"two\"\n  "
        "],\n  {\n    \"one\": \"two\"\n  }\n]\n",
        false}}, // W42U
      {"d77e76c96528a965793e9246a10090465b34a08edd8f1d27d48dd7ff5c25a0e2",
       {"\"%!PS-Adobe-2.0\\n\"\nnull\n", false}}, // W4TN
      {"66a50a9d9a492d7c9674f5c7eb3f923e085da22b817f2230eaceb74248424a79",
       {"{\n  \"a\": \"scalar a\",\n  \"b\": \"scalar a\"\n}\n",
        false}}, // W5VH
      {"ef352176db32daa642df5c6c3d717fb1da0ab5b1947d47e7214dc5b5bf95223a",
       {"{\n  \"foo\": \"\",\n  \"\": \"bar\"\n}\n", false}}, // WZ62
      {"d0a62aaccf63904ca4923f93f59bfc1ccb06055252bf4cf00fa59fdf47afae4f",
       {"{\n  \"key\": \"value\"\n}\n", false}}, // X8DW
      {"e4a3ae35fe2e2631ffeffd6a6005a1f5f88f1923284d8649c84b93c2a1200315",
       {"\"scalar %YAML 1.2\"\n", false}}, // XLQ9
      {"d5d887a109493127b5b6016c0044706ae1975ec9d565490791b021e1a549d225",
       {"{\n  \"Folding\": \"Empty line\\nas a line feed\",\n  \"Chomping\": "
        "\"Clipped empty lines\\n\"\n}\n",
        false}}, // XV9V
      {"5288c5b9f2e73feff3d7139c5dcc190ee8222d202a6a4ae5df3fcd90684544e2",
       {"{\n  \"key\": \"value\"\n}\n", false}}, // Y2GN
      {"d20c0f6969f2abf9b509314cddaeb133b9dff085a88ecf9d0161316613272406",
       {"[\n  [\n    \"name\",\n    \"hr\",\n    \"avg\"\n  ],\n  [\n    "
        "\"Mark McGwire\",\n    65,\n    0.278\n  ],\n  [\n    \"Sammy "
        "Sosa\",\n    63,\n    0.288\n  ]\n]\n",
        false}}, // YD5X
      {"f70654517e8597b7cfa86cbc068170190e5182a3a9de0158670bb48b0060e5ab",
       {"{\n  \"literal\": \"value\\n\",\n  \"folded\": \"value\\n\"\n}\n",
        false}}, // Z67P
      {"23bae94fc4688454728dfd755083d531aa4380bbec6c7203a75fbd1772082fda",
       {"[\n  \"bar\"\n]\n", false}}, // Z9M4
      {"6261b16f75a5cf4d470b7c2656de9fd2665259f2256ef8f1249bb8b9152bc7b7",
       {"{\n  \"Mark McGwire\": {\n    \"hr\": 65,\n    \"avg\": 0.278\n  },\n "
        " \"Sammy Sosa\": {\n    \"hr\": 63,\n    \"avg\": 0.288\n  }\n}\n",
        false}}, // ZF4X
      {"14a0f0e4a44c4fc7abea568346da762bcc8c1636fa9fb352d933b946ec8bced4",
       {"{\n  \"a\": \"b\",\n  \"c\": \"d\"\n}\n", false}}, // ZH7C
      {"e4c842dfbfbed89cae134a50881e5c4c6e487a79b2c7e9ee4b1645eab84504cf",
       {"{\n  \"key\": [\n    [\n      [\n        \"value\"\n      ]\n    ]\n  "
        "]\n}\n",
        false}}, // ZK9H
      {"e2b5971a198f3d96a31e04bf1bf525a81f2de30bd6e34febcb1be14216e6e14f",
       {"{\n  \"a\": 1,\n  \"b\": null,\n  \"c\": 3\n}\n", false}}, // ZWK4
      {"0afb5ce15e48b71fed546017708e80070c1a95903103a2077ce59d15200939ff",
       {"", true}}, // 236B
      {"6bbeb82a925fca5f844e31810f0f3c32c390efacc77ac12d7fdb64c4ef9e2a8a",
       {"", true}}, // 2CMS
      {"b62ecc842277ba19ea5a1f5ab34e8fb4afe5ebaa70ef1b6611598cf1e4729a16",
       {"", true}}, // 3HFZ
      {"188889c0c4ddd59e8bff90f22dc5795ca9ed736c53c6d6e119c7a1c90226ca06",
       {"", true}}, // 4EJS
      {"59067b457b360040a8d36930d08c70f324554a66a407ce794437be3ef4efc02e",
       {"", true}}, // 4H7K
      {"531fb4bbd421aa57559651f716eb65a31fdefaf939ae84e1af27a157ec103020",
       {"", true}}, // 4HVU
      {"bffc4a078d24d4206d922ba82b5646f5cdbff83ecd745200361ee71d41e0a227",
       {"", true}}, // 4JVG
      {"2b84fef10ddcb1bb5f13da8a1edd286ec6f96bcbda5106cb2040d333ad2586dd",
       {"", true}}, // 55WF
      {"336e29d067e3b8e37235a9fb1c9057640256fe63edac49abd3ba62b9c9e245f6",
       {"", true}}, // 5LLU
      {"77dd4201ab4fdf6944d843fc1c8dea834ac009b0517bf55e5353503855a190f2",
       {"", true}}, // 5TRB
      {"efd9e918819f8972037de4ca904855af6450dff1a6db399cfb0ea912ae44d5f6",
       {"", true}}, // 5U3A
      {"4911f54eaff172929e023851268b7dd57c8fd402774e0c35f1f1175bb0b2c7b2",
       {"", true}}, // 62EZ
      {"4c9dca197aa53235537073d581b14861b605bfe6bec208bc199b6ffbaa4dfae7",
       {"", true}}, // 6JTT
      {"fad6dfe7b3f151569b9e0012cb11e3d31e8c113aab22d83aa8ad0d55c6bbadee",
       {"", true}}, // 6S55
      {"05b8ed105c0cfba0b514ae9cbda61c441abe78fc52bb1c0f70b3a25e34c98e8d",
       {"", true}}, // 7LBH
      {"240db2def0bbae5a0d6bd76869830978d309311916f9519debb395805039d73a",
       {"", true}}, // 7MNF
      {"989d12119839315d766f0fff50acccdaa54120a35bc4638d700540a6ac8eac32",
       {"", true}}, // 8XDJ
      {"aea49ec7b15cab59bac0e8a194d08b4cdc0ab0da49c810d6743ae1ec65a76cae",
       {"", true}}, // 9C9N
      {"4408d1693c296d3b8c90d3b8ff00af62d4b3caee96e3168030c646aefe1d23ec",
       {"", true}}, // 9CWY
      {"4113d23d98440233bdfad04d2df18daeadc9f0e950e07b34a64b6b2e3469432d",
       {"", true}}, // 9HCY
      {"1ac8dd1e84a18f8f1bb69469e970e172fc857d980f16ff20377f1e9015270b5e",
       {"", true}}, // 9JBA
      {"00f007f94f6e614a29f4d1a089edd9050be9bc7d65968c5ed045d30275e362fd",
       {"", true}}, // 9KBC
      {"1b39a99b0535cbf5c1c8514274209f0854d548b70bd995ff077a5ccb56d5b4a6",
       {"", true}}, // 9MAG
      {"00986d6fd791ee780cacf545ae2d4c6528bde361c38befda3a161469d9dc0d3b",
       {"", true}}, // 9MMA
      {"21c8b22e7bebda1164c681294b4ca4cfc49ab36b077c3f572aef17c076ecf76e",
       {"", true}}, // B63P
      {"d076e21723d3fc70a7dd29e80b09af3879bd85ab5b18c0e78214bb38f6bfc7c8",
       {"", true}}, // BD7L
      {"7d3f4f835956c6a432c1d013c6adcaf186f5e8eace288ff6f9554193377cd58b",
       {"", true}}, // BF9H
      {"aaa00868cafa406c20138ade74be9ba01d943148add8cec1c1b2110736d1e45f",
       {"", true}}, // BS4K
      {"bcbeb102ec27b0a2cb1c6d292e05b8596fdf22a4b8c0c0720b797fecba7a25b4",
       {"", true}}, // C2SP
      {"3bcea691b250457f0dde4f598a87ee578765c82d1ebe391fa5a4d0d355594678",
       {"", true}}, // CML9
      {"417ce8fad00c6d2bc7a31f49d4f6e6e0fcca737c210b9ff9f3f682088be9e425",
       {"", true}}, // CQ3W
      {"3e0d749788bc494007c051730749a3c5354ab5d27c01c374f2134c6dddc220fb",
       {"", true}}, // CTN5
      {"bf025eb3c7d41d5ff85c0abda9d1e3596b9f72fa033b85edbae92d665cc4a2e8",
       {"", true}}, // CVW2
      {"bc5056978a5a51b3821c1791e0cf3fd88fd2be740d69b0b5672fde90c86a68d2",
       {"", true}}, // CXX2
      {"8e1fcd1515a19ef0aeb9f2789c8c40580d1fb42cf914bd7764e84c9803f88ff9",
       {"", true}}, // D49Q
      {"981ad9614cfff3feae2f2c9962059586630f454b4d3117204f46149487f15bdb",
       {"", true}}, // DK4H
      {"5d14780330517bc6a33e35b04459878d21119c2977994cb91d65338c44f087bd",
       {"", true}}, // DMG6
      {"0fbd1b0c9a02cc686ddb1d599bdbaf7d888975fddc0baace0be68b2c220ad5e1",
       {"", true}}, // EB22
      {"d49041047fba9e0045ca98386f4573f252db5944346c62b36565a55c999bd8bb",
       {"", true}}, // EW3V
      {"6350f108a26e0be362d1767d5d63adc2f3bdb9315a26361209ca04247eafade1",
       {"", true}}, // G5U8
      {"4dcdd5688bfbb39f831a2265ad1631031f2f2d8acbb8f4aafdc4258391090be1",
       {"", true}}, // G7JE
      {"ceef5332d68f98308a43a4e0cbfe88b4d0164ee071a26e0e53132043443e24d1",
       {"", true}}, // G9HC
      {"3d01fde3bfbaac369ac0ac7cb03e1acb0a84969824c71a6f73129c586b292c2b",
       {"", true}}, // GDY7
      {"08e23852a6c2cf3c305bbb21aa9cff3ae0336611da90c18a927df96b6cce321a",
       {"", true}}, // GT5M
      {"e73380a04a8f481396f23c58bdcf1fb00682f181b65a8a7848b3739e08f327b2",
       {"", true}}, // H7J7
      {"7566422fde341aa0f47d5fc4f80291ec63d348869c59e713f2c29edd9393439c",
       {"", true}}, // H7TQ
      {"53b439d97e469a6bd82a6411bcf0ce32590fc26092ed82791d068bd2d1155452",
       {"", true}}, // HRE5
      {"71c945395fafb87f25a6598ed895ba2e49108526a32cbe8bf28983a7ab2d17d4",
       {"", true}}, // HU3P
      {"e498857349fa9200ae2d213e63f2e718f55bdbe0d8b2a9c31511d567da27e3f8",
       {"", true}}, // JKF3
      {"baf0171c25b2986d745d685697dfb2f04d47b5b91c7b2dfaf585d2b364db3bc3",
       {"", true}}, // JY7Z
      {"4e18db69502ccf34b1dc2fa8eeafec3391dcd845557fc5e6910be00337cd7387",
       {"", true}}, // KS4U
      {"a3d264693e3ad0b10bf68c10b3b4c20a4ae8bffbb173b26a2cb262810da9e93e",
       {"", true}}, // LHL4
      {"13f0402526f16ee06c2cace604a999c013cca57c0d0f89a7d0b09821ce2f09cb",
       {"", true}}, // N4JP
      {"d7b8b4cf01da10d38ddd73ebb0aad728cd5d466315031482eeb1053b4f3bbd2e",
       {"", true}}, // N782
      {"3c95ea489a69c8850ddc95cdff830baa2e430a0097eef7274aaabd754cdf09bb",
       {"", true}}, // P2EQ
      {"ad39bbab6cbd2e1cfb481f9af26d3f12ef0f77fd2c41b8611d246d910106bc22",
       {"", true}}, // Q4CL
      {"6f2c6cc22daf60abc12357f755604d21200903e5b56021fc0738e9c7f47286ae",
       {"", true}}, // QB6E
      {"7fb217c3eb53681d9dceb1f409cd57066b3189736f32ca34246974c03a980680",
       {"", true}}, // QLJ7
      {"623fd717720d663166dc89510683d3ded5da6d72c25dafbf6429380c80d30d5f",
       {"", true}}, // RHX7
      {"522842f6ce79d417cad708095dcfe85f4c97916bf2222b869f2ae43c3b513554",
       {"", true}}, // RXY3
      {"db0aa9af46d93e5f02d68fb5347b8a56c9a95d05ea3960c819191921231c1d26",
       {"", true}}, // S4GJ
      {"8f109570c63e7caf93a8dea7b369d54f4499e38f0dd2df7d7145a939f7f4ea22",
       {"", true}}, // S98Z
      {"6c1051719b693effef8a390673516835cb1aab6f21d5eeee3006c3d2dd9d63da",
       {"", true}}, // SF5V
      {"a590006bee39c2e6dfd88ea289618ad6f00a790fba3453d3aa70e4a29cc8261b",
       {"", true}}, // SR86
      {"c3a239120ba0e11c36fde37b7762b160aa69d2f0561862be781f881a581290c6",
       {"", true}}, // SU5Z
      {"de097b3c09b1bde6bd0ab60b8c2bf9f4d72d33d30207221b5c3a518ec6654b0b",
       {"", true}}, // SU74
      {"58f226c9db5b7ec334224c8e3e1bfca63c41d7e76656102ceb7ffb7a4fdd8f14",
       {"", true}}, // SY6V
      {"37e2a3c10d0a58ce79ae0c9dd8d08881a9973b9a244fd9194a5db52cb8d781a6",
       {"", true}}, // T833
      {"50794480db7b4d1b7db082da35a3b7ce179dfac50eb8ec9db5710c29df039559",
       {"", true}}, // TD5N
      {"cc0ff79a6a25f764d54f55d08a95dd68e4f9be2442cdf9b69be345448eecd746",
       {"", true}}, // U44R
      {"e16fd94da5b6bcadc48fc6e549fe760921aa44e7371016ea1248210fc62fafcd",
       {"", true}}, // U99R
      {"96bb894301ba029e1a83229d0efe8e4edef1d57ae3f3da642f52403b0fb586b6",
       {"", true}}, // W9L4
      {"2cdb1ec1a3f5daf904735d506fd256b7256e90c98588d0a79c1943b1096f593a",
       {"", true}}, // X4QW
      {"20784e1348ef257f35acdc0b6d8cbcb8dd4b88ef1f8e2997541021f66d234109",
       {"", true}}, // YJV2
      {"ee13b9a4bcf1cd471e824b8a7a5f783e5ceda01952bd69c324d9939848d06a04",
       {"", true}}, // ZCZ6
      {"fe4493a0f9b651ad22cb0f15d672e4d52213751bce1f0781204aefe84f366eee",
       {"", true}}, // ZL4Z
      {"a6d4d47d26bebb3e8ecda173fbee8ed237ac21fc3188c421dfdddb54025f4a05",
       {"", true}}, // ZVH3
      {"152a245a83d6190333fbcd47100d3e29411ad4222e1fcc6638ee4b00bc330135",
       {"", true}}, // ZXT5
  };
  return instance;
}
} // namespace sourcemeta::core::yaml::cheat
// NOLINTEND
#endif
