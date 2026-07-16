// AUTO-GENERATED. Do not edit.

struct KnownCase {
  const char *input;
  const char *expand_context;
  const char *expected;
  const char *error_code;
  const char *pointer;
  sourcemeta::core::JSONLDVersion version;
};

static const std::array<KnownCase, 83> kReferenceCases{{
    KnownCase{.input = R"jld({})jld",
              .expand_context = nullptr,
              .expected = R"jld([])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/foo":"bar"})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/foo":[{"@value":"bar"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@id":"http://example.com/a","http://example.com/foo":"bar"})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"@id":"http://example.com/a","http://example.com/foo":[{"@value":"bar"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@id":"http://example.com/a","@type":"http://example.com/T","http://example.com/foo":"bar"})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"@id":"http://example.com/a","@type":["http://example.com/T"],"http://example.com/foo":[{"@value":"bar"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/foo":["a","b"]})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/foo":[{"@value":"a"},{"@value":"b"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/foo":1})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/foo":[{"@value":1}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/foo":true})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/foo":[{"@value":true}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"foo":"bar"})jld",
              .expand_context = nullptr,
              .expected = R"jld([])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"name":"http://example.com/name"},"name":"John"})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/name":[{"@value":"John"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@vocab":"http://example.com/"},"name":"John"})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/name":[{"@value":"John"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"ex":"http://example.com/"},"ex:name":"John"})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/name":[{"@value":"John"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"knows":{"@id":"http://example.com/knows","@type":"@id"}},"knows":"http://example.com/jane"})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/knows":[{"@id":"http://example.com/jane"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"born":{"@id":"http://example.com/born","@type":"http://www.w3.org/2001/XMLSchema#date"}},"born":"1990-01-01"})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/born":[{"@value":"1990-01-01","@type":"http://www.w3.org/2001/XMLSchema#date"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@language":"en","name":"http://example.com/name"},"name":"John"})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/name":[{"@value":"John","@language":"en"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/foo":{"@list":["a","b"]}})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/foo":[{"@list":[{"@value":"a"},{"@value":"b"}]}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"p":"http://example.com/p"},"p":{"@value":"v","@direction":"rtl"}})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/p":[{"@value":"v"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"p":"http://example.com/p"},"p":"v","@included":{"@id":"http://example.com/other"}})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/p":[{"@value":"v"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"nest":{"@id":"@nest","@context":{"nest":{"@id":"http://example.com/nest"}}}},"nest":{"http://example.com/foo":"bar"}})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/foo":[{"@value":"bar"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"e":{"@id":"http://example.com/e","@type":"@json","@container":"@list"}},"e":42})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/e":[{"@list":[{"@value":42,"@type":"@json"}]}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"p":{"@id":"http://example.com/p","@type":"@id"}},"p":"@foo"})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/p":[{"@id":null}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@id":"http://example.com/n","@type":"@foo","http://example.com/p":"v"})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"@id":"http://example.com/n","http://example.com/p":[{"@value":"v"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@graph":"scalar"})jld",
              .expand_context = nullptr,
              .expected = R"jld([])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":"https://example.com/remote-base","@id":"relative-node","http://example.com/p":"v"})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"@id":"http://doc.example/relative-node","http://example.com/p":[{"@value":"v"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"p":{"@id":"http://example.com/p","@container":"@language","@context":{"@direction":"rtl"}}},"p":{"en":"hello"}})jld",
              .expand_context = nullptr,
              .expected = R"jld([{"http://example.com/p":[{"@value":"hello","@language":"en","@direction":"rtl"}]}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"term":{"@id":"term:term"}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Cyclic IRI mapping)jld",
              .pointer = R"jld(/@context/term)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"":"http://example.com/"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld(/@context)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@type":"http://example.com/"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Keyword redefinition)jld",
              .pointer = R"jld(/@context/@type)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":[{"@protected":true,"a":"http://example.com/a"},{"a":"http://example.com/b"}]})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld(/@context/1/a)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@protected":"yes"}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @protected value)jld",
              .pointer = R"jld(/@context/a/@protected)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":true}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid IRI mapping)jld",
              .pointer = R"jld(/@context/a/@id)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"@context"}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid keyword alias)jld",
              .pointer = R"jld(/@context/a/@id)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@reverse":"http://example.com/a","@id":"http://example.com/b"}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid reverse property)jld",
              .pointer = R"jld(/@context/a/@reverse)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@type":true}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid type mapping)jld",
              .pointer = R"jld(/@context/a/@type)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@container":"@unknown"}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid container mapping)jld",
              .pointer = R"jld(/@context/a/@container)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@language":true}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid language mapping)jld",
              .pointer = R"jld(/@context/a/@language)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@prefix":"yes"}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @prefix value)jld",
              .pointer = R"jld(/@context/a/@prefix)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@nest":"@id"}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @nest value)jld",
              .pointer = R"jld(/@context/a/@nest)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@context":{"b":{"@id":true}}}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid scoped context)jld",
              .pointer = R"jld(/@context/a/@context)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":true})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid local context)jld",
              .pointer = R"jld(/@context)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@version":2.0}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @version value)jld",
              .pointer = R"jld(/@context/@version)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@propagate":"yes"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @propagate value)jld",
              .pointer = R"jld(/@context/@propagate)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@import":true}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @import value)jld",
              .pointer = R"jld(/@context/@import)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@base":true}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid base IRI)jld",
              .pointer = R"jld(/@context/@base)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@vocab":true}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid vocab mapping)jld",
              .pointer = R"jld(/@context/@vocab)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@language":true}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid default language)jld",
              .pointer = R"jld(/@context/@language)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@direction":"sideways"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid base direction)jld",
              .pointer = R"jld(/@context/@direction)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@version":1.1}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Processing mode conflict)jld",
              .pointer = R"jld(/@context/@version)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@protected":true}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid context entry)jld",
              .pointer = R"jld(/@context)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":"https://example.com/missing"})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Loading remote context failed)jld",
              .pointer = R"jld(/@context)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":"https://example.com/no-context"})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid remote context)jld",
              .pointer = R"jld(/@context)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":"https://example.com/recursive"})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Recursive context inclusion)jld",
              .pointer = R"jld(/@context)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"id":"@id"},"@id":"http://example.com/a","id":"http://example.com/b"})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Colliding keywords)jld",
              .pointer = R"jld(/id)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@id":true})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @id value)jld",
              .pointer = R"jld(/@id)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@type":true})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid type value)jld",
              .pointer = R"jld(/@type)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/p":{"@value":"x","@type":"http://example.com/t","@language":"en"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid value object)jld",
              .pointer = R"jld(/http:~1~1example.com~1p)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/p":{"@value":"x","@language":true}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid language-tagged string)jld",
              .pointer = R"jld(/http:~1~1example.com~1p/@language)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/p":{"@value":1,"@language":"en"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid language-tagged value)jld",
              .pointer = R"jld(/http:~1~1example.com~1p)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/p":{"@value":"x","@type":"_:b"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid typed value)jld",
              .pointer = R"jld(/http:~1~1example.com~1p)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/p":{"@value":{"a":1}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid value object value)jld",
              .pointer = R"jld(/http:~1~1example.com~1p)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/p":{"@list":["a"],"@id":"http://example.com/x"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid set or list object)jld",
              .pointer = R"jld(/http:~1~1example.com~1p)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/p":{"@index":true,"@value":"x"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @index value)jld",
              .pointer = R"jld(/http:~1~1example.com~1p/@index)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@reverse":"foo"})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @reverse value)jld",
              .pointer = R"jld(/@reverse)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@reverse":{"http://example.com/p":{"@value":"x"}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid reverse property value)jld",
              .pointer = R"jld(/@reverse)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@included":{"@value":"x"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @included value)jld",
              .pointer = R"jld(/@included)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"nest":"@nest"},"nest":{"@value":"x"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @nest value)jld",
              .pointer = R"jld(/nest)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"http://example.com/p":{"@list":[{"@list":["a"]}]}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(List of lists)jld",
              .pointer = R"jld(/http:~1~1example.com~1p/@list)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"p":"http://example.com/p"},"p":{"@value":"v","@direction":"up"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid base direction)jld",
              .pointer = R"jld(/p/@direction)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"none":"@none"},"@reverse":{"none":"x"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid reverse property map)jld",
              .pointer = R"jld(/@reverse/none)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"type":"@type"},"@type":"http://example.com/A","type":"http://example.com/B"})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Colliding keywords)jld",
              .pointer = R"jld(/type)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"p":"http://example.com/p"},"p":{"@language":42}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid language-tagged string)jld",
              .pointer = R"jld(/p/@language)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@protected":true}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld(/@context/a/@protected)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@nest":"@nest"}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld(/@context/a/@nest)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"ex":"http://example.com/","ex:foo":{"@id":"http://example.com/foo","@prefix":true}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld(/@context/ex:foo/@prefix)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@direction":"up"}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid base direction)jld",
              .pointer = R"jld(/@context/a/@direction)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@container":["@id","@language"]}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid container mapping)jld",
              .pointer = R"jld(/@context/a/@container)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@bogus":true}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld(/@context/a)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@type":{"@container":"@id"}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Keyword redefinition)jld",
              .pointer = R"jld(/@context/@type)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@version":1.5}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @version value)jld",
              .pointer = R"jld(/@context/@version)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":[{"@base":null},{"@base":"relative/path"}]})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid base IRI)jld",
              .pointer = R"jld(/@context/1/@base)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":[{"term":{"@id":null,"@protected":true}},{"term":"http://example.com/x"}]})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld(/@context/1/term)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@list":["foo"],"@id":"http://example.com/bar"})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid set or list object)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"@import":"https://example.com/unknown"}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Loading remote context failed)jld",
              .pointer = R"jld(/@context/@import)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({"@context":{"a":{"@id":"http://example.com/a","@container":["@set","@set"]}}})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid container mapping)jld",
              .pointer = R"jld(/@context/a/@container)jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
}};

static const std::array<KnownCase, 385> kVendorCases{{
    KnownCase{.input = R"jld({"@id": "http://example.org/test#example"})jld",
              .expand_context = nullptr,
              .expected = R"jld([ ]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "t1": "http://example.com/t1",
    "t2": "http://example.com/t2",
    "term1": "http://example.com/term1",
    "term2": "http://example.com/term2",
    "term3": "http://example.com/term3",
    "term4": "http://example.com/term4",
    "term5": "http://example.com/term5"
  },
  "@id": "http://example.com/id1",
  "@type": "t1",
  "term1": "v1",
  "term2": {"@value": "v2", "@type": "t2"},
  "term3": {"@value": "v3", "@language": "en"},
  "term4": 4,
  "term5": [50, 51]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.com/id1",
  "@type": ["http://example.com/t1"],
  "http://example.com/term1": [{"@value": "v1"}],
  "http://example.com/term2": [{"@value": "v2", "@type": "http://example.com/t2"}],
  "http://example.com/term3": [{"@value": "v3", "@language": "en"}],
  "http://example.com/term4": [{"@value": 4}],
  "http://example.com/term5": [{"@value": 50}, {"@value": 51}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@id": "http://example.org/id",
  "http://example.org/property": null,
  "regularJson": {
    "nonJsonLd": "property",
    "deep": [{
      "foo": "bar"
    }, {
      "bar": "foo"
    }]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([ ]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "mylist1": {"@id": "http://example.com/mylist1", "@container": "@list"},
    "mylist2": {"@id": "http://example.com/mylist2", "@container": "@list"},
    "myset2": {"@id": "http://example.com/myset2", "@container": "@set"},
    "myset3": {"@id": "http://example.com/myset3", "@container": "@set"}
  },
  "@id": "http://example.org/id",
  "mylist1": { "@list": [ ] },
  "mylist2": "one item",
  "myset2": { "@set": [ ] },
  "myset3": [ "v1" ],
  "http://example.org/list1": { "@list": [ null ] },
  "http://example.org/list2": { "@list": [ {"@value": null} ] },
  "http://example.org/set1": { "@set": [ ] },
  "http://example.org/set2": { "@set": [ null ] },
  "http://example.org/set3": [ ],
  "http://example.org/set4": [ null ],
  "http://example.org/set5": "one item",
  "http://example.org/property": { "@list": "one item" }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.org/id",
  "http://example.com/mylist1": [ { "@list": [ ] } ],
  "http://example.com/mylist2": [ { "@list": [ {"@value": "one item"} ] } ],
  "http://example.com/myset2": [ ],
  "http://example.com/myset3": [ {"@value": "v1"} ],
  "http://example.org/list1": [ { "@list": [ ] } ],
  "http://example.org/list2": [ { "@list": [ ] } ],
  "http://example.org/set1": [ ],
  "http://example.org/set2": [ ],
  "http://example.org/set3": [ ],
  "http://example.org/set4": [ ],
  "http://example.org/set5": [ {"@value": "one item"} ],
  "http://example.org/property": [ { "@list": [ {"@value": "one item"} ] } ]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name",
    "homepage": {
      "@id": "http://xmlns.com/foaf/0.1/homepage",
      "@type": "@id"
    },
    "know": "http://xmlns.com/foaf/0.1/knows",
    "@iri": "@id"
  },
  "@id": "#me",
  "know": [
    {
      "@id": "http://example.com/bob#me",
      "name": "Bob",
      "homepage": "http://example.com/bob"
    }, {
      "@id": "http://example.com/alice#me",
      "name": "Alice",
      "homepage": "http://example.com/alice"
    }
  ]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "https://w3c.github.io/json-ld-api/tests/expand/0005-in.jsonld#me",
  "http://xmlns.com/foaf/0.1/knows": [
    {
      "@id": "http://example.com/bob#me",
      "http://xmlns.com/foaf/0.1/name": [{"@value": "Bob"}],
      "http://xmlns.com/foaf/0.1/homepage": [{
        "@id": "http://example.com/bob"
      }]
    }, {
      "@id": "http://example.com/alice#me",
      "http://xmlns.com/foaf/0.1/name": [{"@value": "Alice"}],
      "http://xmlns.com/foaf/0.1/homepage": [{
        "@id": "http://example.com/alice"
      }]
    }
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "http://example.org/test#property1": {
      "@type": "@id"
    },
    "http://example.org/test#property2": {
      "@type": "@id"
    },
    "uri": "@id"
  },
  "http://example.org/test#property1": {
    "http://example.org/test#property4": "foo",
    "uri": "http://example.org/test#example2"
  },
  "http://example.org/test#property2": "http://example.org/test#example3",
  "http://example.org/test#property3": {
    "uri": "http://example.org/test#example4"
  },
  "uri": "http://example.org/test#example1"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.org/test#example1",
  "http://example.org/test#property1": [{
    "@id": "http://example.org/test#example2",
    "http://example.org/test#property4": [{"@value": "foo"}]
  }],
  "http://example.org/test#property2": [{
    "@id": "http://example.org/test#example3"
  }],
  "http://example.org/test#property3": [{
    "@id": "http://example.org/test#example4"
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "ex": "http://example.org/vocab#",
    "ex:date": {
      "@type": "xsd:dateTime"
    },
    "ex:parent": {
      "@type": "@id"
    },
    "xsd": "http://www.w3.org/2001/XMLSchema#"
  },
  "@id": "http://example.org/test#example1",
  "ex:date": "2011-01-25T00:00:00Z",
  "ex:embed": {
    "@id": "http://example.org/test#example2",
    "ex:parent": "http://example.org/test#example1"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.org/test#example1",
  "http://example.org/vocab#date": [{
    "@value": "2011-01-25T00:00:00Z",
    "@type": "http://www.w3.org/2001/XMLSchema#dateTime"
  }],
  "http://example.org/vocab#embed": [{
    "@id": "http://example.org/test#example2",
    "http://example.org/vocab#parent": [{
      "@id": "http://example.org/test#example1"
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "ex": "http://example.org/vocab#"
  },
  "@id": "http://example.org/test",
  "ex:test": { "@value": "test",  "@language": "en" },
  "ex:drop-lang-only": { "@language": "en" },
  "ex:keep-full-value": { "@value": "only value" }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.org/test",
    "http://example.org/vocab#test": [ { "@value": "test", "@language": "en" } ],
    "http://example.org/vocab#keep-full-value": [ {"@value": "only value"} ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "authored": {
      "@id": "http://example.org/vocab#authored",
      "@type": "@id"
    },
    "contains": {
      "@id": "http://example.org/vocab#contains",
      "@type": "@id"
    },
    "contributor": "http://purl.org/dc/elements/1.1/contributor",
    "description": "http://purl.org/dc/elements/1.1/description",
    "name": "http://xmlns.com/foaf/0.1/name",
    "title": {
      "@id": "http://purl.org/dc/elements/1.1/title"
    }
  },
  "@graph": [
    {
      "@id": "http://example.org/test#chapter",
      "description": "Fun",
      "title": "Chapter One"
    },
    {
      "@id": "http://example.org/test#jane",
      "authored": "http://example.org/test#chapter",
      "name": "Jane"
    },
    {
      "@id": "http://example.org/test#john",
      "name": "John"
    },
    {
      "@id": "http://example.org/test#library",
      "contains": {
        "@id": "http://example.org/test#book",
        "contains": "http://example.org/test#chapter",
        "contributor": "Writer",
        "title": "My Book"
      }
    }
  ]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.org/test#chapter",
    "http://purl.org/dc/elements/1.1/description": [{"@value": "Fun"}],
    "http://purl.org/dc/elements/1.1/title": [{"@value": "Chapter One"}]
  },
  {
    "@id": "http://example.org/test#jane",
    "http://example.org/vocab#authored": [{
      "@id": "http://example.org/test#chapter"
    }],
    "http://xmlns.com/foaf/0.1/name": [{"@value": "Jane"}]
  },
  {
    "@id": "http://example.org/test#john",
    "http://xmlns.com/foaf/0.1/name": [{"@value": "John"}]
  },
  {
    "@id": "http://example.org/test#library",
    "http://example.org/vocab#contains": [{
      "@id": "http://example.org/test#book",
      "http://example.org/vocab#contains": [{
        "@id": "http://example.org/test#chapter"
      }],
      "http://purl.org/dc/elements/1.1/contributor": [{"@value": "Writer"}],
      "http://purl.org/dc/elements/1.1/title": [{"@value": "My Book"}]
    }]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "d": "http://purl.org/dc/elements/1.1/",
    "e": "http://example.org/vocab#",
    "f": "http://xmlns.com/foaf/0.1/",
    "xsd": "http://www.w3.org/2001/XMLSchema#"
  },
  "@id": "http://example.org/test",
  "e:bool": true,
  "e:int": 123
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.org/test",
  "http://example.org/vocab#bool": [{"@value": true}],
  "http://example.org/vocab#int": [{"@value": 123}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "dc11": "http://purl.org/dc/elements/1.1/",
    "ex": "http://example.org/vocab#",
    "ex:contains": {
      "@type": "@id"
    },
    "xsd": "http://www.w3.org/2001/XMLSchema#"
  },
  "@id": "http://example.org/test#book",
  "dc11:title": "Title",
  "ex:contains": "http://example.org/test#chapter"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.org/test#book",
  "http://example.org/vocab#contains": [{
    "@id": "http://example.org/test#chapter"
  }],
  "http://purl.org/dc/elements/1.1/title": [{"@value": "Title"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "dc11": "http://purl.org/dc/elements/1.1/",
    "ex": "http://example.org/vocab#",
    "ex:authored": {
      "@type": "@id"
    },
    "ex:contains": {
      "@type": "@id"
    },
    "foaf": "http://xmlns.com/foaf/0.1/",
    "xsd": "http://www.w3.org/2001/XMLSchema#"
  },
  "@graph": [
    {
      "@id": "http://example.org/test#chapter",
      "dc11:description": "Fun",
      "dc11:title": "Chapter One"
    },
    {
      "@id": "http://example.org/test#jane",
      "ex:authored": "http://example.org/test#chapter",
      "foaf:name": "Jane"
    },
    {
      "@id": "http://example.org/test#john",
      "foaf:name": "John"
    },
    {
      "@id": "http://example.org/test#library",
      "ex:contains": {
        "@id": "http://example.org/test#book",
        "dc11:contributor": "Writer",
        "dc11:title": "My Book",
        "ex:contains": "http://example.org/test#chapter"
      }
    }
  ]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.org/test#chapter",
    "http://purl.org/dc/elements/1.1/description": [{"@value": "Fun"}],
    "http://purl.org/dc/elements/1.1/title": [{"@value": "Chapter One"}]
  },
  {
    "@id": "http://example.org/test#jane",
    "http://example.org/vocab#authored": [{
      "@id": "http://example.org/test#chapter"
    }],
    "http://xmlns.com/foaf/0.1/name": [{"@value": "Jane"}]
  },
  {
    "@id": "http://example.org/test#john",
    "http://xmlns.com/foaf/0.1/name": [{"@value": "John"}]
  },
  {
    "@id": "http://example.org/test#library",
    "http://example.org/vocab#contains": [{
      "@id": "http://example.org/test#book",
      "http://example.org/vocab#contains": [{
        "@id": "http://example.org/test#chapter"
      }],
      "http://purl.org/dc/elements/1.1/contributor": [{"@value": "Writer"}],
      "http://purl.org/dc/elements/1.1/title": [{"@value": "My Book"}]
    }]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld([{
  "@id": "http://example.com/id1",
  "@type": ["http://example.com/t1"],
  "http://example.com/term1": ["v1"],
  "http://example.com/term2": [{"@value": "v2", "@type": "http://example.com/t2"}],
  "http://example.com/term3": [{"@value": "v3", "@language": "en"}],
  "http://example.com/term4": [4],
  "http://example.com/term5": [50, 51]
}])jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.com/id1",
  "@type": ["http://example.com/t1"],
  "http://example.com/term1": [{"@value": "v1"}],
  "http://example.com/term2": [{"@value": "v2", "@type": "http://example.com/t2"}],
  "http://example.com/term3": [{"@value": "v3", "@language": "en"}],
  "http://example.com/term4": [{"@value": 4}],
  "http://example.com/term5": [{"@value": 50}, {"@value": 51}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "ex": "http://example.org/test#",
    "property1": {
      "@id": "http://example.org/test#property1",
      "@type": "@id"
    },
    "property2": {
      "@id": "ex:property2",
      "@type": "@id"
    },
    "uri": "@id",
    "set": "@set",
    "value": "@value",
    "type": "@type",
    "xsd": "http://www.w3.org/2001/XMLSchema#"
  },
  "property1": {
    "uri": "ex:example2",
    "http://example.org/test#property4": "foo"
  },
  "property2": "http://example.org/test#example3",
  "http://example.org/test#property3": {
    "uri": "http://example.org/test#example4"
  },
  "ex:property4": {
    "uri": "ex:example4",
    "ex:property5": [
      {
        "set": [
          {
          "value": "2012-03-31",
          "type": "xsd:date"
          }
        ]
      }
    ]
  },
  "ex:property6": [
    {
      "set": [
        {
        "value": null,
        "type": "xsd:date"
        }
      ]
    }
  ],
  "uri": "http://example.org/test#example1"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/test#property1": [
      {
        "@id": "http://example.org/test#example2",
        "http://example.org/test#property4": [
          {"@value": "foo"}
        ]
      }
    ],
    "http://example.org/test#property2": [
      { "@id": "http://example.org/test#example3" }
    ],
    "http://example.org/test#property3": [
      { "@id": "http://example.org/test#example4" }
    ],
    "http://example.org/test#property4": [
      {
        "@id": "http://example.org/test#example4",
        "http://example.org/test#property5": [
          {
            "@value": "2012-03-31",
            "@type": "http://www.w3.org/2001/XMLSchema#date"
          }
        ]
      }
    ],
    "http://example.org/test#property6": [],
    "@id": "http://example.org/test#example1"
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "mylist1": {"@id": "http://example.com/mylist1", "@container": "@list"},
    "mylist2": {"@id": "http://example.com/mylist2", "@container": "@list"},
    "myset1": {"@id": "http://example.com/myset1", "@container": "@set" },
    "myset2": {"@id": "http://example.com/myset2", "@container": "@set" },
    "myset3": {"@id": "http://example.com/myset3", "@container": "@set" }
  },
  "@id": "http://example.org/id",
  "mylist1": [],
  "myset1": { "@set": [] },
  "myset2": [ { "@set": [] }, [], { "@set": [ null ] }, [ null ] ],
  "myset3": [ { "@set": [ "hello", "this" ] }, "will", { "@set": [ "be", "collapsed" ] } ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.org/id",
    "http://example.com/mylist1": [ { "@list": [] } ],
    "http://example.com/myset1": [ ],
    "http://example.com/myset2": [ ],
    "http://example.com/myset3": [
      {"@value": "hello"},
      {"@value": "this"},
      {"@value": "will"},
      {"@value": "be"},
      {"@value": "collapsed"} ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "myproperty": { "@id": "http://example.com/myproperty" },
    "mylist1": {"@id": "http://example.com/mylist1", "@container": "@list"},
    "mylist2": {"@id": "http://example.com/mylist2", "@container": "@list"},
    "myset1": {"@id": "http://example.com/myset1", "@container": "@set" },
    "myset2": {"@id": "http://example.com/myset2", "@container": "@set" }
  },
  "@id": "http://example.org/id1",
  "mylist1": [],
  "mylist2": [ 2, "hi" ],
  "myset1": { "@set": [] },
  "myset2": [ { "@set": [] }, [], { "@set": [ null ] }, [ null ] ],
  "myproperty": {
    "@context": null,
    "@id": "http://example.org/id2",
    "mylist1": [],
    "mylist2": [ 2, "hi" ],
    "myset1": { "@set": [] },
    "myset2": [ { "@set": [] }, [], { "@set": [ null ] }, [ null ] ],
    "http://example.org/myproperty2": "ok"
  },
  "http://example.com/emptyobj": {
    "@context": null,
    "mylist1": [],
    "mylist2": [ 2, "hi" ],
    "myset1": { "@set": [] },
    "myset2": [ { "@set": [] }, [], { "@set": [ null ] }, [ null ] ]
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.org/id1",
    "http://example.com/mylist1": [ { "@list": [] } ],
    "http://example.com/mylist2": [
      { "@list": [ {"@value": 2}, {"@value": "hi"} ] }
    ],
    "http://example.com/myset1": [ ],
    "http://example.com/myset2": [ ],
    "http://example.com/myproperty": [
      {
        "@id": "http://example.org/id2",
        "http://example.org/myproperty2": [ {"@value": "ok"} ]
      }
    ],
    "http://example.com/emptyobj": [ { } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "authored": {
      "@id": "http://example.org/vocab#authored",
      "@type": "@id"
    },
    "contains": {
      "@id": "http://example.org/vocab#contains",
      "@type": "@id"
    },
    "contributor": "http://purl.org/dc/elements/1.1/contributor",
    "description": "http://purl.org/dc/elements/1.1/description",
    "name": "http://xmlns.com/foaf/0.1/name",
    "title": {
      "@id": "http://purl.org/dc/elements/1.1/title"
    },
    "id": "@id",
    "data": "@graph"
  },
  "data": [
    {
      "id": "http://example.org/test#chapter",
      "description": "Fun",
      "title": "Chapter One"
    },
    {
      "@id": "http://example.org/test#jane",
      "authored": "http://example.org/test#chapter",
      "name": "Jane"
    },
    {
      "id": "http://example.org/test#john",
      "name": "John"
    },
    {
      "id": "http://example.org/test#library",
      "contains": {
        "@id": "http://example.org/test#book",
        "contains": "http://example.org/test#chapter",
        "contributor": "Writer",
        "title": "My Book"
      }
    }
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.org/test#chapter",
    "http://purl.org/dc/elements/1.1/description": [{"@value": "Fun"}],
    "http://purl.org/dc/elements/1.1/title": [{"@value": "Chapter One"}]
  },
  {
    "@id": "http://example.org/test#jane",
    "http://example.org/vocab#authored": [{
      "@id": "http://example.org/test#chapter"
    }],
    "http://xmlns.com/foaf/0.1/name": [{"@value": "Jane"}]
  },
  {
    "@id": "http://example.org/test#john",
    "http://xmlns.com/foaf/0.1/name": [{"@value": "John"}]
  },
  {
    "@id": "http://example.org/test#library",
    "http://example.org/vocab#contains": [{
      "@id": "http://example.org/test#book",
      "http://example.org/vocab#contains": [{
        "@id": "http://example.org/test#chapter"
      }],
      "http://purl.org/dc/elements/1.1/contributor": [{"@value": "Writer"}],
      "http://purl.org/dc/elements/1.1/title": [{"@value": "My Book"}]
    }]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "ex": "http://example.org/vocab#",
    "@language": "en",
    "de": { "@id": "ex:german", "@language": "de" },
    "nolang": { "@id": "ex:nolang", "@language": null }
  },
  "@id": "http://example.org/test",
  "ex:test-default": [
    "hello",
    1,
    true
  ],
  "de": [
    "hallo",
    2,
    true
  ],
  "nolang": [
    "no language",
    3,
    false
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.org/test",
    "http://example.org/vocab#test-default": [ { "@value": "hello", "@language": "en" }, { "@value": 1 }, { "@value": true } ],
    "http://example.org/vocab#german": [ { "@value": "hallo", "@language": "de" }, { "@value": 2 }, { "@value": true } ],
    "http://example.org/vocab#nolang": [ {"@value": "no language"}, { "@value": 3 }, { "@value": false } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "myproperty": "http://example.com/myproperty"
  },
  "myproperty": { "@value" : null }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([ ]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "authored": {
      "@id": "http://example.org/vocab#authored",
      "@type": "@id"
    },
    "contains": {
      "@id": "http://example.org/vocab#contains",
      "@type": "@id"
    },
    "contributor": "http://purl.org/dc/elements/1.1/contributor",
    "description": "http://purl.org/dc/elements/1.1/description",
    "name": "http://xmlns.com/foaf/0.1/name",
    "title": {
      "@id": "http://purl.org/dc/elements/1.1/title"
    }
  },
  "@graph": [
    {
      "@id": "http://example.org/test#jane",
      "name": "Jane",
      "authored": {
        "@graph": [
          {
            "@id": "http://example.org/test#chapter1",
            "description": "Fun",
            "title": "Chapter One"
          },
          {
            "@id": "http://example.org/test#chapter2",
            "description": "More fun",
            "title": "Chapter Two"
          }
        ]
      }
    },
    {
      "@id": "http://example.org/test#john",
      "name": "John"
    },
    {
      "@id": "http://example.org/test#library",
      "contains": {
        "@id": "http://example.org/test#book",
        "contains": "http://example.org/test#chapter",
        "contributor": "Writer",
        "title": "My Book"
      }
    }
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.org/test#jane",
    "http://xmlns.com/foaf/0.1/name": [ {"@value": "Jane"} ],
    "http://example.org/vocab#authored": [
      {
        "@graph": [
          {
            "@id": "http://example.org/test#chapter1",
            "http://purl.org/dc/elements/1.1/description": [ {"@value": "Fun"} ],
            "http://purl.org/dc/elements/1.1/title": [ {"@value": "Chapter One"} ]
          },
          {
            "@id": "http://example.org/test#chapter2",
            "http://purl.org/dc/elements/1.1/description": [ {"@value": "More fun"} ],
            "http://purl.org/dc/elements/1.1/title": [ {"@value": "Chapter Two"} ]
          }
        ]
      }
    ]
  },
  {
    "@id": "http://example.org/test#john",
    "http://xmlns.com/foaf/0.1/name": [ {"@value": "John"} ]
  },
  {
    "@id": "http://example.org/test#library",
    "http://example.org/vocab#contains": [
      {
        "@id": "http://example.org/test#book",
        "http://example.org/vocab#contains": [ { "@id": "http://example.org/test#chapter" } ],
        "http://purl.org/dc/elements/1.1/contributor": [ {"@value": "Writer"} ],
        "http://purl.org/dc/elements/1.1/title": [ {"@value": "My Book"} ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "authored": {
      "@id": "http://example.org/vocab#authored",
      "@type": "@id"
    },
    "contains": {
      "@id": "http://example.org/vocab#contains",
      "@type": "@id"
    },
    "contributor": "http://purl.org/dc/elements/1.1/contributor",
    "description": "http://purl.org/dc/elements/1.1/description",
    "name": "http://xmlns.com/foaf/0.1/name",
    "title": {
      "@id": "http://purl.org/dc/elements/1.1/title"
    }
  },
  "title": "My first graph",
  "@graph": [
    {
      "@id": "http://example.org/test#jane",
      "name": "Jane",
      "authored": {
        "@graph": [
          {
            "@id": "http://example.org/test#chapter1",
            "description": "Fun",
            "title": "Chapter One"
          },
          {
            "@id": "http://example.org/test#chapter2",
            "description": "More fun",
            "title": "Chapter Two"
          },
          {
            "@id": "http://example.org/test#chapter3",
            "title": "Chapter Three"
          }
        ]
      }
    },
    {
      "@id": "http://example.org/test#john",
      "name": "John"
    },
    {
      "@id": "http://example.org/test#library",
      "contains": {
        "@id": "http://example.org/test#book",
        "contains": "http://example.org/test#chapter",
        "contributor": "Writer",
        "title": "My Book"
      }
    }
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://purl.org/dc/elements/1.1/title": [ {"@value": "My first graph"} ],
    "@graph": [
      {
        "@id": "http://example.org/test#jane",
        "http://xmlns.com/foaf/0.1/name": [ {"@value": "Jane"} ],
        "http://example.org/vocab#authored": [
          {
            "@graph": [
              {
                "@id": "http://example.org/test#chapter1",
                "http://purl.org/dc/elements/1.1/description": [ {"@value": "Fun"} ],
                "http://purl.org/dc/elements/1.1/title": [ {"@value": "Chapter One"} ]
              },
              {
                "@id": "http://example.org/test#chapter2",
                "http://purl.org/dc/elements/1.1/description": [ {"@value": "More fun"} ],
                "http://purl.org/dc/elements/1.1/title": [ {"@value": "Chapter Two"} ]
              },
              {
                "@id": "http://example.org/test#chapter3",
                "http://purl.org/dc/elements/1.1/title": [ {"@value": "Chapter Three"} ]
              }
            ]
          }
        ]
      },
      {
        "@id": "http://example.org/test#john",
        "http://xmlns.com/foaf/0.1/name": [ {"@value": "John"} ]
      },
      {
        "@id": "http://example.org/test#library",
        "http://example.org/vocab#contains": [
          {
            "@id": "http://example.org/test#book",
            "http://example.org/vocab#contains": [ { "@id": "http://example.org/test#chapter" } ],
            "http://purl.org/dc/elements/1.1/contributor": [ {"@value": "Writer"} ],
            "http://purl.org/dc/elements/1.1/title": [ {"@value": "My Book"} ]
          }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": "http://example.com/term",
    "@language": "en"
  },
  "term": "v"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/term": [{"@value": "v", "@language": "en"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "xsd": "http://www.w3.org/2001/XMLSchema#",
    "idlist": {"@id": "http://example.com/idlist", "@container": "@list", "@type": "@id"},
    "datelist": {"@id": "http://example.com/datelist", "@container": "@list", "@type": "xsd:date"},
    "idset": {"@id": "http://example.com/idset", "@container": "@set", "@type": "@id"},
    "dateset": {"@id": "http://example.com/dateset", "@container": "@set", "@type": "xsd:date"},
    "idprop": {"@id": "http://example.com/idprop", "@type": "@id" },
    "dateprop": {"@id": "http://example.com/dateprop", "@type": "xsd:date" },
    "idprop2": {"@id": "http://example.com/idprop2", "@type": "@id" },
    "dateprop2": {"@id": "http://example.com/dateprop2", "@type": "xsd:date" }
  },
  "idlist": ["http://example.org/id"],
  "datelist": ["2012-04-12"],
  "idprop": {"@list": ["http://example.org/id"]},
  "dateprop": {"@list": ["2012-04-12"]},
  "idset": ["http://example.org/id"],
  "dateset": ["2012-04-12"],
  "idprop2": {"@set": ["http://example.org/id"]},
  "dateprop2": {"@set": ["2012-04-12"]}
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.com/idlist": [{"@list": [{"@id": "http://example.org/id"}]}],
    "http://example.com/datelist": [{"@list": [{"@value": "2012-04-12","@type": "http://www.w3.org/2001/XMLSchema#date"}]}],
    "http://example.com/idprop": [{"@list": [{"@id": "http://example.org/id"}]}],
    "http://example.com/dateprop": [{"@list": [{"@value": "2012-04-12","@type": "http://www.w3.org/2001/XMLSchema#date"}]}],
    "http://example.com/idset": [{"@id": "http://example.org/id"}],
    "http://example.com/dateset": [{"@value": "2012-04-12","@type": "http://www.w3.org/2001/XMLSchema#date"}],
    "http://example.com/idprop2": [{"@id": "http://example.org/id"}],
    "http://example.com/dateprop2": [{"@value": "2012-04-12","@type": "http://www.w3.org/2001/XMLSchema#date"}]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    {
      "name": "http://xmlns.com/foaf/0.1/name",
      "homepage": {"@id": "http://xmlns.com/foaf/0.1/homepage","@type": "@id"}
    },
    {"ical": "http://www.w3.org/2002/12/cal/ical#"}
  ],
  "@id": "http://example.com/speakers#Alice",
  "name": "Alice",
  "homepage": "http://xkcd.com/177/",
  "ical:summary": "Alice Talk",
  "ical:location": "Lyon Convention Centre, Lyon, France"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.com/speakers#Alice",
  "http://xmlns.com/foaf/0.1/name": [{"@value": "Alice"}],
  "http://xmlns.com/foaf/0.1/homepage": [{"@id": "http://xkcd.com/177/"}],
  "http://www.w3.org/2002/12/cal/ical#summary": [{"@value": "Alice Talk"}],
  "http://www.w3.org/2002/12/cal/ical#location": [{"@value": "Lyon Convention Centre, Lyon, France"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "foo": "http://example.com/foo/",
    "foo:bar": "http://example.com/foo/bar",
    "bar": {"@id": "foo:bar", "@type": "@id"},
    "_": "http://example.com/underscore/"
  },
  "@type": [ "foo", "foo:bar", "_" ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": [
    "http://example.com/foo/",
    "http://example.com/foo/bar",
    "http://example.com/underscore/"
  ]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "http://www.w3.org/1999/02/22-rdf-syntax-ns#type": {"@id": "@type", "@type": "@id"}
  },
  "@graph": [
    {
      "@id": "http://example.com/a",
      "http://www.w3.org/1999/02/22-rdf-syntax-ns#type": "http://example.com/b"
    }, {
      "@id": "http://example.com/c",
      "http://www.w3.org/1999/02/22-rdf-syntax-ns#type": [
        "http://example.com/d",
        "http://example.com/e"
      ]
    }, {
      "@id": "http://example.com/f",
      "http://www.w3.org/1999/02/22-rdf-syntax-ns#type": "http://example.com/g"
    }
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/a",
    "@type": [
      "http://example.com/b"
    ]
  },
  {
    "@id": "http://example.com/c",
    "@type": [
      "http://example.com/d",
      "http://example.com/e"
    ]
  },
  {
    "@id": "http://example.com/f",
    "@type": [
      "http://example.com/g"
    ]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": {
    "mylist": {"@id": "http://example.com/mylist", "@container": "@list"},
    "myset": {"@id": "http://example.com/myset", "@container": "@set"}
  },
  "@id": "http://example.org/id",
  "mylist": [1, 2, 2, 3],
  "myset": [1, 2, 2, 3]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.org/id",
  "http://example.com/mylist": [{
    "@list": [
      {"@value": 1},
      {"@value": 2},
      {"@value": 2},
      {"@value": 3}
    ]
  }],
  "http://example.com/myset": [
    {"@value": 1},
    {"@value": 2},
    {"@value": 2},
    {"@value": 3}
  ]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/vocab#",
    "date": { "@type": "dateTime" }
  },
  "@id": "example1",
  "@type": "test",
  "date": "2011-01-25T00:00:00Z",
  "embed": {
    "@id": "example2",
    "expandedDate": { "@value": "2012-08-01T00:00:00Z", "@type": "dateTime" }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "https://w3c.github.io/json-ld-api/tests/expand/example1",
    "@type": [ "http://example.org/vocab#test" ],
    "http://example.org/vocab#date": [
      {
        "@value": "2011-01-25T00:00:00Z",
        "@type": "http://example.org/vocab#dateTime"
      }
    ],
    "http://example.org/vocab#embed": [
      {
        "@id": "https://w3c.github.io/json-ld-api/tests/expand/example2",
        "http://example.org/vocab#expandedDate": [
          {
            "@value": "2012-08-01T00:00:00Z",
            "@type": "http://example.org/vocab#dateTime"
          }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "links": { "@id": "http://www.example.com/link", "@type": "@id", "@container": "@list" }
  },
  "@id": "relativeIris",
  "@type": [
    "link",
    "#fragment-works",
    "?query=works",
    "./",
    "../",
    "../parent",
    "../../../parent-parent-eq-root",
    "../../../../../still-root",
    "../.././.././../../too-many-dots",
    "/absolute",
    "//example.org/scheme-relative"
  ],
  "links": [
    "link",
    "#fragment-works",
    "?query=works",
    "./",
    "../",
    "../parent",
    "../../../parent-parent-eq-root",
    "./../../../useless/../../../still-root",
    "../.././.././../../too-many-dots",
    "/absolute",
    "//example.org/scheme-relative"
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "https://w3c.github.io/json-ld-api/tests/expand/relativeIris",
    "@type": [
      "https://w3c.github.io/json-ld-api/tests/expand/link",
      "https://w3c.github.io/json-ld-api/tests/expand/0029-in.jsonld#fragment-works",
      "https://w3c.github.io/json-ld-api/tests/expand/0029-in.jsonld?query=works",
      "https://w3c.github.io/json-ld-api/tests/expand/",
      "https://w3c.github.io/json-ld-api/tests/",
      "https://w3c.github.io/json-ld-api/tests/parent",
      "https://w3c.github.io/parent-parent-eq-root",
      "https://w3c.github.io/still-root",
      "https://w3c.github.io/too-many-dots",
      "https://w3c.github.io/absolute",
      "https://example.org/scheme-relative"
    ],
    "http://www.example.com/link": [
      {
        "@list": [
          {"@id": "https://w3c.github.io/json-ld-api/tests/expand/link"},
          {"@id": "https://w3c.github.io/json-ld-api/tests/expand/0029-in.jsonld#fragment-works"},
          {"@id": "https://w3c.github.io/json-ld-api/tests/expand/0029-in.jsonld?query=works"},
          {"@id": "https://w3c.github.io/json-ld-api/tests/expand/"},
          {"@id": "https://w3c.github.io/json-ld-api/tests/"},
          {"@id": "https://w3c.github.io/json-ld-api/tests/parent"},
          {"@id": "https://w3c.github.io/parent-parent-eq-root"},
          {"@id": "https://w3c.github.io/still-root"},
          {"@id": "https://w3c.github.io/too-many-dots"},
          {"@id": "https://w3c.github.io/absolute"},
          {"@id": "https://example.org/scheme-relative"}
        ]
      }
    ]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "vocab": "http://example.com/vocab/",
    "label": {
      "@id": "vocab:label",
      "@container": "@language"
    }
  },
  "@id": "http://example.com/queen",
  "label": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ]
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/queen",
    "http://example.com/vocab/label":
    [
      {
        "@value": "Die Königin",
        "@language": "de"
      }, {
        "@value": "Ihre Majestät",
        "@language": "de"
      }, {
        "@value": "The Queen",
        "@language": "en"
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "ex": "http://example.org/vocab#",
    "xsd": "http://www.w3.org/2001/XMLSchema#",
    "ex:integer": { "@type": "xsd:integer" },
    "ex:double": { "@type": "xsd:double" },
    "ex:boolean": { "@type": "xsd:boolean" }
  },
  "@id": "http://example.org/test#example1",
  "ex:integer": 1,
  "ex:double": 123.45,
  "ex:boolean": true
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.org/test#example1",
    "http://example.org/vocab#integer": [ {
      "@value": 1,
      "@type": "http://www.w3.org/2001/XMLSchema#integer"
    } ],
    "http://example.org/vocab#double": [ {
      "@value": 123.45,
      "@type": "http://www.w3.org/2001/XMLSchema#double"
    } ],
    "http://example.org/vocab#boolean": [ {
      "@value": true,
      "@type": "http://www.w3.org/2001/XMLSchema#boolean"
    } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://xmlns.com/foaf/0.1/",
    "from": null,
    "university": { "@id": null }
  },
  "@id": "http://me.markus-lanthaler.com/",
  "name": "Markus Lanthaler",
  "from": "Italy",
  "university": "TU Graz"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
   {
      "@id": "http://me.markus-lanthaler.com/",
      "http://xmlns.com/foaf/0.1/name": [
         {
            "@value": "Markus Lanthaler"
         }
      ]
   }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.com/vocab#",
    "homepage": {
      "@type": "@id"
    },
    "created_at": {
      "@type": "http://www.w3.org/2001/XMLSchema#date"
    }
  },
  "name": "Markus Lanthaler",
  "homepage": "http://www.markus-lanthaler.com/",
  "created_at": "2012-10-28"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
   "http://example.com/vocab#name": [{
      "@value": "Markus Lanthaler"
   }],
   "http://example.com/vocab#homepage": [{
      "@id": "http://www.markus-lanthaler.com/"
   }],
   "http://example.com/vocab#created_at": [{
      "@value": "2012-10-28",
      "@type": "http://www.w3.org/2001/XMLSchema#date"
   }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.com/vocab/",
    "colliding": "http://example.com/vocab/collidingTerm"
  },
  "@id": "http://example.com/IriCollissions",
  "colliding": [
    "value 1",
    2
  ],
  "collidingTerm": [
    3,
    "four"
  ],
  "http://example.com/vocab/collidingTerm": 5
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
   "@id": "http://example.com/IriCollissions",
   "http://example.com/vocab/collidingTerm": [
      {
         "@value": "value 1"
      }, {
         "@value": 2
      }, {
         "@value": 3
      }, {
         "@value": "four"
      },
      {
         "@value": 5
      }
   ]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.com/vocab/",
    "@language": "it",
    "label": {
      "@container": "@language"
    }
  },
  "@id": "http://example.com/queen",
  "label": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ]
  },
  "http://example.com/vocab/label": [
    "Il re",
    { "@value": "The king", "@language": "en" }
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
   "@id": "http://example.com/queen",
   "http://example.com/vocab/label": [
      {
         "@value": "Il re",
         "@language": "it"
      }, {
         "@value": "The king",
         "@language": "en"
      }, {
         "@value": "Die Königin",
         "@language": "de"
      }, {
         "@value": "Ihre Majestät",
         "@language": "de"
      }, {
         "@value": "The Queen",
         "@language": "en"
      }
   ]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "property": "http://example.com/property",
    "indexContainer": { "@id": "http://example.com/container", "@container": "@index" }
  },
  "@id": "http://example.org/indexTest",
  "indexContainer": {
    "A": [
      {
        "@id": "http://example.org/nodeWithoutIndexA"
      },
      {
        "@id": "http://example.org/nodeWithIndexA",
        "@index": "this overrides the 'A' index from the container"
      },
      1,
      true,
      false,
      null,
      "simple string A",
      {
        "@value": "typed literal A",
        "@type": "http://example.org/type"
      },
      {
        "@value": "language-tagged string A",
        "@language": "en"
      }
    ],
    "B": "simple string B",
    "C": [
      {
        "@id": "http://example.org/nodeWithoutIndexC"
      },
      {
        "@id": "http://example.org/nodeWithIndexC",
        "@index": "this overrides the 'C' index from the container"
      },
      3,
      true,
      false,
      null,
      "simple string C",
      {
        "@value": "typed literal C",
        "@type": "http://example.org/type"
      },
      {
        "@value": "language-tagged string C",
        "@language": "en"
      }
    ]
  },
  "property": [
    {
      "@id": "http://example.org/nodeWithoutIndexProp"
    },
    {
      "@id": "http://example.org/nodeWithIndexProp",
      "@index": "prop"
    },
    {
      "@value": 3,
      "@index": "prop"
    },
    {
      "@value": true,
      "@index": "prop"
    },
    {
      "@value": false,
      "@index": "prop"
    },
    {
      "@value": null,
      "@index": "prop"
    },
    "simple string no index",
    {
      "@value": "typed literal Prop",
      "@type": "http://example.org/type",
      "@index": "prop"
    },
    {
      "@value": "language-tagged string Prop",
      "@language": "en",
      "@index": "prop"
    }
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
   {
      "@id": "http://example.org/indexTest",
      "http://example.com/container": [
         {
            "@id": "http://example.org/nodeWithoutIndexA",
            "@index": "A"
         },
         {
            "@id": "http://example.org/nodeWithIndexA",
            "@index": "this overrides the 'A' index from the container"
         },
         {
            "@value": 1,
            "@index": "A"
         },
         {
            "@value": true,
            "@index": "A"
         },
         {
            "@value": false,
            "@index": "A"
         },
         {
            "@value": "simple string A",
            "@index": "A"
         },
         {
            "@value": "typed literal A",
            "@type": "http://example.org/type",
            "@index": "A"
         },
         {
            "@value": "language-tagged string A",
            "@language": "en",
            "@index": "A"
         },
         {
            "@value": "simple string B",
            "@index": "B"
         },
         {
            "@id": "http://example.org/nodeWithoutIndexC",
            "@index": "C"
         },
         {
            "@id": "http://example.org/nodeWithIndexC",
            "@index": "this overrides the 'C' index from the container"
         },
         {
            "@value": 3,
            "@index": "C"
         },
         {
            "@value": true,
            "@index": "C"
         },
         {
            "@value": false,
            "@index": "C"
         },
         {
            "@value": "simple string C",
            "@index": "C"
         },
         {
            "@value": "typed literal C",
            "@type": "http://example.org/type",
            "@index": "C"
         },
         {
            "@value": "language-tagged string C",
            "@language": "en",
            "@index": "C"
         }
      ],
      "http://example.com/property": [
         {
            "@id": "http://example.org/nodeWithoutIndexProp"
         },
         {
            "@id": "http://example.org/nodeWithIndexProp",
            "@index": "prop"
         },
         {
            "@value": 3,
            "@index": "prop"
         },
         {
            "@value": true,
            "@index": "prop"
         },
         {
            "@value": false,
            "@index": "prop"
         },
         {
            "@value": "simple string no index"
         },
         {
            "@value": "typed literal Prop",
            "@type": "http://example.org/type",
            "@index": "prop"
         },
         {
            "@value": "language-tagged string Prop",
            "@language": "en",
            "@index": "prop"
         }
      ]
   }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name"
  },
  "@id": "http://example.com/people/markus",
  "name": "Markus Lanthaler",
  "@reverse": {
    "http://xmlns.com/foaf/0.1/knows": {
      "@id": "http://example.com/people/dave",
      "name": "Dave Longley"
    }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/people/markus",
    "@reverse": {
      "http://xmlns.com/foaf/0.1/knows": [
        {
          "@id": "http://example.com/people/dave",
          "http://xmlns.com/foaf/0.1/name": [ { "@value": "Dave Longley" } ]
        }
      ]
    },
    "http://xmlns.com/foaf/0.1/name": [ { "@value": "Markus Lanthaler" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": "_:term",
    "termId": { "@id": "term", "@type": "@id" }
  },
  "@id": "_:term",
  "@type": "_:term",
  "term": [
    {
      "@id": "_:term",
      "@type": "term"
    },
    {
      "@id": "_:Bx",
      "term": "term"
    },
    "plain value",
    {
      "@id": "_:term"
    }
  ],
  "termId": [
    {
      "@id": "_:term",
      "@type": "term"
    },
    {
      "@id": "_:Cx",
      "term": "termId"
    },
    "term:AppendedToBlankNode",
    "_:termAppendedToBlankNode",
    "relativeIri",
    {
      "@id": "_:term"
    }
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "_:term",
    "@type": [
      "_:term"
    ],
    "_:term": [
      {
        "@id": "_:term",
        "@type": [
          "_:term"
        ]
      },
      {
        "@id": "_:Bx",
        "_:term": [
          {
            "@value": "term"
          }
        ]
      },
      {
        "@value": "plain value"
      },
      {
        "@id": "_:term"
      },
      {
        "@id": "_:term",
        "@type": [
          "_:term"
        ]
      },
      {
        "@id": "_:Cx",
        "_:term": [
          {
            "@value": "termId"
          }
        ]
      },
      {
        "@id": "_:termAppendedToBlankNode"
      },
      {
        "@id": "_:termAppendedToBlankNode"
      },
      {
        "@id": "https://w3c.github.io/json-ld-api/tests/expand/relativeIri"
      },
      {
        "@id": "_:term"
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name",
    "knows": "http://xmlns.com/foaf/0.1/knows"
  },
  "@id": "http://example.com/people/markus",
  "name": "Markus Lanthaler",
  "@reverse": {
    "knows": {
      "@id": "http://example.com/people/dave",
      "name": "Dave Longley"
    }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/people/markus",
    "@reverse": {
      "http://xmlns.com/foaf/0.1/knows": [
        {
          "@id": "http://example.com/people/dave",
          "http://xmlns.com/foaf/0.1/name": [ { "@value": "Dave Longley" } ]
        }
      ]
    },
    "http://xmlns.com/foaf/0.1/name": [ { "@value": "Markus Lanthaler" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "vocab": "http://example.com/vocab/",
    "label": {
      "@id": "vocab:label",
      "@container": "@language"
    },
    "indexes": {
      "@id": "vocab:index",
      "@container": "@index"
    }
  },
  "@id": "http://example.com/queen",
  "label": [
    "The Queen"
  ],
  "indexes":
  [
    "No",
    "indexes",
    { "@id": "asTheValueIsntAnObject" }
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/queen",
    "http://example.com/vocab/label":
    [
      {
        "@value": "The Queen"
      }
    ],
    "http://example.com/vocab/index":
    [
      {
        "@value": "No"
      },
      {
        "@value": "indexes"
      },
      {
        "@id": "https://w3c.github.io/json-ld-api/tests/expand/asTheValueIsntAnObject"
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "property": "http://example.com/property",
    "nested": "http://example.com/nested",
    "@language": "en"
  },
  "property": "this is English",
  "nested": {
    "@context": {
      "@language": null
    },
    "property": "and this is a plain string"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.com/property": [ { "@value": "this is English", "@language": "en" } ],
    "http://example.com/nested": [
      {
        "http://example.com/property": [ { "@value": "and this is a plain string" } ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name",
    "isKnownBy": { "@reverse": "http://xmlns.com/foaf/0.1/knows" }
  },
  "@id": "http://example.com/people/markus",
  "name": "Markus Lanthaler",
  "isKnownBy": {
    "@id": "http://example.com/people/dave",
    "name": "Dave Longley"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/people/markus",
    "@reverse": {
      "http://xmlns.com/foaf/0.1/knows": [
        {
          "@id": "http://example.com/people/dave",
          "http://xmlns.com/foaf/0.1/name": [ { "@value": "Dave Longley" } ]
        }
      ]
    },
    "http://xmlns.com/foaf/0.1/name": [ { "@value": "Markus Lanthaler" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name",
    "isKnownBy": { "@reverse": "http://xmlns.com/foaf/0.1/knows" }
  },
  "@id": "http://example.com/people/markus",
  "name": "Markus Lanthaler",
  "@reverse": {
    "isKnownBy": [
      {
        "@id": "http://example.com/people/dave",
        "name": "Dave Longley"
      },
      {
        "@id": "http://example.com/people/gregg",
        "name": "Gregg Kellogg"
      }
    ]
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/people/markus",
    "http://xmlns.com/foaf/0.1/knows": [
      {
        "@id": "http://example.com/people/dave",
        "http://xmlns.com/foaf/0.1/name": [ { "@value": "Dave Longley" } ]
      },
      {
        "@id": "http://example.com/people/gregg",
        "http://xmlns.com/foaf/0.1/name": [ { "@value": "Gregg Kellogg" } ]
      }
    ],
    "http://xmlns.com/foaf/0.1/name": [ { "@value": "Markus Lanthaler" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "property": { "@id": "http://example.com/vocab/property", "@language": "de" },
    "indexMap": { "@id": "http://example.com/vocab/indexMap", "@language": "en", "@container": "@index" }
  },
  "@id": "http://example.com/node",
  "property": [
    {
      "@id": "http://example.com/propertyValueNode",
      "indexMap": {
        "expands to english string": "simple string"
      }
    },
    "einfacher String"
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/node",
    "http://example.com/vocab/property": [
      {
        "@id": "http://example.com/propertyValueNode",
        "http://example.com/vocab/indexMap": [
          {
            "@value": "simple string",
            "@language": "en",
            "@index": "expands to english string"
          }
        ]
      },
      {
        "@value": "einfacher String",
        "@language": "de"
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
    "@value": "free-floating value"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([ ]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
    "@graph": [
        { "@id": "http://example.com/free-floating-node" },
        { "@value": "free-floating value object" },
        { "@value": "free-floating value language-tagged string",  "@language": "en" },
        { "@value": "free-floating value typed value",  "@type": "http://example.com/type" },
        "free-floating plain string",
        true,
        false,
        null,
        1,
        1.5
    ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([ ]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
    "@context": {
        "property": "http://example.com/property"
    },
    "@graph": [
        {
            "@set": [
                "free-floating strings in set objects are removed",
                {
                    "@id": "http://example.com/free-floating-node"
                },
                {
                    "@id": "http://example.com/node",
                    "property": "nodes with properties are not removed"
                }
            ]
        },
        {
            "@list": [
                "lists are removed even though they represent an invisible linked structure, they have no real meaning",
                {
                    "@id": "http://example.com/node-in-free-floating-list",
                    "property": "everything inside a free-floating list is removed with the list; also nodes with properties"
                }
            ]
        }
    ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
    {
        "@id": "http://example.com/node",
        "http://example.com/property": [
            {
                "@value": "nodes with properties are not removed"
            }
        ]
    }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": "http://example.com/terms-are-not-considered-in-id",
    "compact-iris": "http://example.com/compact-iris#",
    "property": "http://example.com/property",
    "@vocab": "http://example.org/vocab-is-not-considered-for-id"
  },
  "@id": "term",
  "property": [
    {
      "@id": "compact-iris:are-considered",
      "property": "@id supports the following values: relative, absolute, and compact IRIs"
    },
    {
      "@id": "../parent-node",
      "property": "relative IRIs get resolved against the document's base IRI"
    }
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "https://w3c.github.io/json-ld-api/tests/expand/term",
    "http://example.com/property": [
      {
        "@id": "http://example.com/compact-iris#are-considered",
        "http://example.com/property": [
          { "@value": "@id supports the following values: relative, absolute, and compact IRIs" }
        ]
      },
      {
        "@id": "https://w3c.github.io/json-ld-api/tests/parent-node",
        "http://example.com/property": [
          { "@value": "relative IRIs get resolved against the document's base IRI" }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name",
    "isKnownBy": { "@reverse": "http://xmlns.com/foaf/0.1/knows", "@type": "@id" }
  },
  "@id": "http://example.com/people/markus",
  "name": "Markus Lanthaler",
  "isKnownBy": [
    "http://example.com/people/dave",
    "http://example.com/people/gregg"
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/people/markus",
    "@reverse": {
      "http://xmlns.com/foaf/0.1/knows": [
        {
          "@id": "http://example.com/people/dave"
        },
        {
          "@id": "http://example.com/people/gregg"
        }
      ]
    },
    "http://xmlns.com/foaf/0.1/name": [ { "@value": "Markus Lanthaler" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "issue": { "@id": "http://example.com/issue/", "@type": "@id" },
    "issue:raisedBy": { "@container": "@set" }
  },
  "issue": "/issue/1",
  "issue:raisedBy": "Markus"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.com/issue/": [ { "@id": "https://w3c.github.io/issue/1" } ],
    "http://example.com/issue/raisedBy": [ { "@value": "Markus" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    { "id": "@id" },
    { "url": "id" }
  ],
  "url": "/issue/1",
  "http://example.com/property": "ok"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
   "http://example.com/property": [{
      "@value": "ok"
   }],
   "@id": "https://w3c.github.io/issue/1"
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "property": "vocabRelativeProperty"
  },
  "property": "must expand to http://example.org/vocabRelativeProperty",
  "http://example.org/property": "ok"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/property": [ { "@value": "ok" } ],
    "http://example.org/vocabRelativeProperty": [ { "@value": "must expand to http://example.org/vocabRelativeProperty" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example.org/term", "@type": "@vocab"}
  },
  "term": "http://example.org/enum"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/term": [{"@id": "http://example.org/enum"}]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example.org/term", "@type": "@vocab"},
    "enum": {"@id": "http://example.org/enum"}
  },
  "term": "enum"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/term": [{"@id": "http://example.org/enum"}]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "term": {"@id": "http://example.org/term", "@type": "@vocab"}
  },
  "term": "enum"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/term": [{"@id": "http://example.org/enum"}]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name",
    "homepage": { "@id": "http://xmlns.com/foaf/0.1/homepage", "@type": "@vocab" },
    "link": { "@id": "http://example.com/link", "@type": "@id" },
    "MarkusHomepage": "http://www.markus-lanthaler.com/",
    "relative-iri": "http://example.com/error-if-this-is-used-for-link"
  },
  "@id": "http://me.markus-lanthaler.com/",
  "name": "Markus Lanthaler",
  "homepage": "MarkusHomepage",
  "link": "relative-iri"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://me.markus-lanthaler.com/",
    "http://xmlns.com/foaf/0.1/homepage": [ { "@id": "http://www.markus-lanthaler.com/" } ],
    "http://example.com/link": [ {  "@id": "https://w3c.github.io/json-ld-api/tests/expand/relative-iri" } ],
    "http://xmlns.com/foaf/0.1/name": [ { "@value": "Markus Lanthaler" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": { "@id": "http://example.org/term", "@type": "@vocab" }
  },
  "term": "not-a-term-thus-a-relative-IRI"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/term": [ { "@id": "https://w3c.github.io/json-ld-api/tests/expand/not-a-term-thus-a-relative-IRI" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": { "@id": "http://example.org/term", "@type": "@vocab" },
    "prefix": "http://example.com/vocab#"
  },
  "term": "prefix:suffix"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/term": [ { "@id": "http://example.com/vocab#suffix" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/vocab#"
  },
  "@id": "example-with-vocab",
  "@type": "vocab-prefixed",
  "property": "property expanded using @vocab",
  "embed": {
    "@context": {
      "@vocab": null
    },
    "@id": "example-vocab-reset",
    "@type": "document-relative",
    "property": "@vocab reset, property will be dropped"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "https://w3c.github.io/json-ld-api/tests/expand/example-with-vocab",
    "@type": [ "http://example.org/vocab#vocab-prefixed" ],
    "http://example.org/vocab#embed": [
      {
        "@id": "https://w3c.github.io/json-ld-api/tests/expand/example-vocab-reset",
        "@type": [ "https://w3c.github.io/json-ld-api/tests/expand/document-relative" ]
      }
    ],
    "http://example.org/vocab#property": [ { "@value": "property expanded using @vocab" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "property": "http://example.com/vocab#property"
  },
  "@id": "../document-relative",
  "@type": "#document-relative",
  "property": {
    "@context": {
      "@base": "http://example.org/test/"
    },
    "@id": "../document-base-overwritten",
    "@type": "#document-base-overwritten",
    "property": [
      {
        "@context": null,
        "@id": "../document-relative",
        "@type": "#document-relative",
        "property": "context completely reset, drops property"
      },
      {
        "@context": {
          "@base": null
        },
        "@id": "../document-relative",
        "@type": "#document-relative",
        "property": "only @base is cleared"
      }
    ]
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "https://w3c.github.io/json-ld-api/tests/document-relative",
    "@type": [ "https://w3c.github.io/json-ld-api/tests/expand/0060-in.jsonld#document-relative" ],
    "http://example.com/vocab#property": [
      {
        "@id": "http://example.org/document-base-overwritten",
        "@type": [ "http://example.org/test/#document-base-overwritten" ],
        "http://example.com/vocab#property": [
          {
            "@id": "https://w3c.github.io/json-ld-api/tests/document-relative",
            "@type": [ "https://w3c.github.io/json-ld-api/tests/expand/0060-in.jsonld#document-relative" ]
          },
          {
            "@id": "../document-relative",
            "@type": [ "#document-relative" ],
            "http://example.com/vocab#property": [ { "@value": "only @base is cleared" } ]
          }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "property": {
      "@id": "http://example.com/property",
      "@type": "http://example.com/datatype"
    }
  },
  "property": [ 1, true, false, 5.1 ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.com/property": [
      { "@value": 1, "@type": "http://example.com/datatype" },
      { "@value": true, "@type": "http://example.com/datatype" },
      { "@value": false, "@type": "http://example.com/datatype" },
      { "@value": 5.1, "@type": "http://example.com/datatype" }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@base": "http://example.com/some/deep/directory/and/file#with-a-fragment",
    "links": { "@id": "http://www.example.com/link", "@type": "@id", "@container": "@list" }
  },
  "@id": "relativeIris",
  "@type": [
    "link",
    "#fragment-works",
    "?query=works",
    "./",
    "../",
    "../parent",
    "../../parent-parent-eq-root",
    "../../../../../still-root",
    "../.././.././../../too-many-dots",
    "/absolute",
    "//example.org/scheme-relative"
  ],
  "links": [
    "link",
    "#fragment-works",
    "?query=works",
    "./",
    "../",
    "../parent",
    "../../parent-parent-eq-root",
    "./../../../../../still-root",
    "../.././.././../../too-many-dots",
    "/absolute",
    "//example.org/scheme-relative",
    "//example.org/../scheme-relative",
    "//example.org/.././useless/../../scheme-relative"
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/some/deep/directory/and/relativeIris",
    "@type": [
        "http://example.com/some/deep/directory/and/link",
        "http://example.com/some/deep/directory/and/file#fragment-works",
        "http://example.com/some/deep/directory/and/file?query=works",
        "http://example.com/some/deep/directory/and/",
        "http://example.com/some/deep/directory/",
        "http://example.com/some/deep/directory/parent",
        "http://example.com/some/deep/parent-parent-eq-root",
        "http://example.com/still-root",
        "http://example.com/too-many-dots",
        "http://example.com/absolute",
        "http://example.org/scheme-relative"
    ],
    "http://www.example.com/link": [ {
      "@list": [
        { "@id": "http://example.com/some/deep/directory/and/link" },
        { "@id": "http://example.com/some/deep/directory/and/file#fragment-works" },
        { "@id": "http://example.com/some/deep/directory/and/file?query=works" },
        { "@id": "http://example.com/some/deep/directory/and/" },
        { "@id": "http://example.com/some/deep/directory/" },
        { "@id": "http://example.com/some/deep/directory/parent" },
        { "@id": "http://example.com/some/deep/parent-parent-eq-root" },
        { "@id": "http://example.com/still-root" },
        { "@id": "http://example.com/too-many-dots" },
        { "@id": "http://example.com/absolute" },
        { "@id": "http://example.org/scheme-relative" },
        { "@id": "http://example.org/scheme-relative" },
        { "@id": "http://example.org/scheme-relative" }
      ]
    } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name",
    "isKnownBy": { "@reverse": "http://xmlns.com/foaf/0.1/knows", "@container": "@index" }
  },
  "@id": "http://example.com/people/markus",
  "name": "Markus Lanthaler",
  "isKnownBy": {
    "Dave": {
      "@id": "http://example.com/people/dave",
      "name": "Dave Longley"
    },
    "Gregg": {
      "@id": "http://example.com/people/gregg",
      "name": "Gregg Kellogg"
    }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/people/markus",
    "@reverse": {
      "http://xmlns.com/foaf/0.1/knows": [
        {
          "@id": "http://example.com/people/dave",
          "@index": "Dave",
          "http://xmlns.com/foaf/0.1/name": [ { "@value": "Dave Longley" } ]
        },
        {
          "@id": "http://example.com/people/gregg",
          "@index": "Gregg",
          "http://xmlns.com/foaf/0.1/name": [ { "@value": "Gregg Kellogg" } ]
        }
      ]
    },
    "http://xmlns.com/foaf/0.1/name": [ { "@value": "Markus Lanthaler" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name",
    "isKnownBy": { "@reverse": "http://xmlns.com/foaf/0.1/knows" }
  },
  "@id": "http://example.com/people/markus",
  "name": "Markus Lanthaler",
  "isKnownBy": [
    {
      "name": "Dave Longley"
    },
    {
      "name": "Gregg Kellogg"
    }
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/people/markus",
    "@reverse": {
      "http://xmlns.com/foaf/0.1/knows": [
        {
          "http://xmlns.com/foaf/0.1/name": [ { "@value": "Dave Longley" } ]
        },
        {
          "http://xmlns.com/foaf/0.1/name": [ { "@value": "Gregg Kellogg" } ]
        }
      ]
    },
    "http://xmlns.com/foaf/0.1/name": [ { "@value": "Markus Lanthaler" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name",
    "knows": "http://xmlns.com/foaf/0.1/knows"
  },
  "@id": "http://example.com/people/markus",
  "name": "Markus Lanthaler",
  "@reverse": {
    "knows": {
      "@id": "http://example.com/people/dave",
      "name": "Dave Longley"
    },
    "relative-iri": {
      "@id": "relative-node",
      "name": "Keys that are not mapped to an IRI in a reverse-map are dropped"
    }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/people/markus",
    "@reverse": {
      "http://xmlns.com/foaf/0.1/knows": [
        {
          "@id": "http://example.com/people/dave",
          "http://xmlns.com/foaf/0.1/name": [ { "@value": "Dave Longley" } ]
        }
      ]
    },
    "http://xmlns.com/foaf/0.1/name": [ { "@value": "Markus Lanthaler" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name",
    "knows": "http://xmlns.com/foaf/0.1/knows",
    "@vocab": "http://example.com/vocab/"
  },
  "@id": "http://example.com/people/markus",
  "name": "Markus Lanthaler",
  "@reverse": {
    "knows": {
      "@id": "http://example.com/people/dave",
      "name": "Dave Longley"
    },
    "noTerm": {
      "@id": "relative-node",
      "name": "Compact keys using @vocab"
    }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/people/markus",
    "@reverse": {
      "http://xmlns.com/foaf/0.1/knows": [
        {
          "@id": "http://example.com/people/dave",
          "http://xmlns.com/foaf/0.1/name": [ { "@value": "Dave Longley" } ]
        }
      ],
      "http://example.com/vocab/noTerm": [
        {
          "@id": "https://w3c.github.io/json-ld-api/tests/expand/relative-node",
          "http://xmlns.com/foaf/0.1/name": [ { "@value": "Compact keys using @vocab" } ]
        }
      ]
    },
    "http://xmlns.com/foaf/0.1/name": [ { "@value": "Markus Lanthaler" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "http": "http://example.com/this-prefix-would-overwrite-all-http-iris"
  },
  "@id": "http://example.org/node1",
  "@type": "http://example.org/type",
  "http://example.org/property": "all these IRIs remain unchanged because they are interpreted as absolute IRIs"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.org/node1",
    "@type": ["http://example.org/type"],
    "http://example.org/property": [
      { "@value": "all these IRIs remain unchanged because they are interpreted as absolute IRIs" }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "_": "http://example.com/this-prefix-would-overwrite-all-blank-node-identifiers"
  },
  "@id": "_:node1",
  "@type": "_:type",
  "_:property": "all these IRIs remain unchanged because they are interpreted as blank node identifiers"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "_:node1",
    "@type": [ "_:type" ],
    "_:property": [
      { "@value": "all these IRIs remain unchanged because they are interpreted as blank node identifiers" }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "rdfs": "http://www.w3.org/2000/01/rdf-schema#",
    "rdfs:subClassOf": { "@id": "rdfs:subClassOf", "@type": "@id" }
  },
  "@id": "http://example.com/vocab#class",
  "@type": "rdfs:Class",
  "rdfs:subClassOf": "http://example.com/vocab#someOtherClass"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/vocab#class",
    "@type": [ "http://www.w3.org/2000/01/rdf-schema#Class" ],
    "http://www.w3.org/2000/01/rdf-schema#subClassOf": [
      { "@id": "http://example.com/vocab#someOtherClass"}
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "prefix": "http://www.example.org/vocab#",
    "prefix:foo": "prefix:foo"
  },
  "@id": "http://example.com/vocab#id",
  "@type": "prefix:Class",
  "prefix:foo": "bar"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/vocab#id",
    "@type": [ "http://www.example.org/vocab#Class" ],
    "http://www.example.org/vocab#foo": [
      { "@value": "bar"}
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    {
      "v": "http://example.com/vocab#",
      "v:term": "v:somethingElse",
      "v:termId": { "@id": "v:somethingElseId" }
    },
    {
      "v:term": "v:term",
      "v:termId": { "@id": "v:termId" }
    }
  ],
  "v:term": "value of v:term",
  "v:termId": "value of v:termId"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.com/vocab#term": [
      { "@value": "value of v:term" }
    ],
    "http://example.com/vocab#termId": [
      { "@value": "value of v:termId" }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": [
    {
      "v": "http://example.com/vocab#",
      "term": "v:somethingElse"
    },
    {
      "@vocab": "http://example.com/anotherVocab#",
      "term": "term"
    }
  ],
  "term": "value of term"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.com/anotherVocab#term": [
      { "@value": "value of term" }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@id": "ex:node1",
  "owl:sameAs": {
    "@id": "ex:node2",
    "rdfs:label": "Node 2",
    "link": "ex:node3",
    "@context": {
      "rdfs": "http://www.w3.org/2000/01/rdf-schema#"
    }
  },
  "@context": {
    "ex": "http://example.org/",
    "owl": "http://www.w3.org/2002/07/owl#",
    "link": { "@id": "ex:link", "@type": "@id" }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.org/node1",
    "http://www.w3.org/2002/07/owl#sameAs": [
      {
        "@id": "http://example.org/node2",
        "http://example.org/link": [
          { "@id": "http://example.org/node3" }
        ],
        "http://www.w3.org/2000/01/rdf-schema#label": [ { "@value": "Node 2" } ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "ex": "http://example.org/",
    "owl": "http://www.w3.org/2002/07/owl#",
    "link": {
      "@id": "ex:link",
      "@type": "@id"
    }
  },
  "owl:sameAs": {
    "@context": {
      "rdfs": "http://www.w3.org/2000/01/rdf-schema#"
    },
    "rdfs:label": "Node 2",
    "link": "ex:node3",
    "@id": "ex:node2"
  },
  "@id": "ex:node1"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.org/node1",
    "http://www.w3.org/2002/07/owl#sameAs": [
      {
        "@id": "http://example.org/node2",
        "http://example.org/link": [
          { "@id": "http://example.org/node3" }
        ],
        "http://www.w3.org/2000/01/rdf-schema#label": [ { "@value": "Node 2" } ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "_:"
  },
  "@id": "ex:node1",
  "b1": "blank node property 1",
  "b2": "blank node property 1"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "ex:node1",
    "_:b1": [ { "@value": "blank node property 1" } ],
    "_:b2": [ { "@value": "blank node property 1" } ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@id": "relative-iri",
  "http://prop": "value"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example/base/relative-iri",
  "http://prop": [{"@value": "value"}]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@id": "http://example.com/id1",
  "@type": "t1",
  "term1": "v1",
  "term2": {"@value": "v2", "@type": "t2"},
  "term3": {"@value": "v3", "@language": "en"},
  "term4": 4,
  "term5": [50, 51]
}
)jld",
              .expand_context = R"jld({"t1": "http://example.com/t1", "t2": "http://example.com/t2", "term1": "http://example.com/term1", "term2": "http://example.com/term2", "term3": "http://example.com/term3", "term4": "http://example.com/term4", "term5": "http://example.com/term5"})jld",
              .expected = R"jld([{
  "@id": "http://example.com/id1",
  "@type": ["http://example.com/t1"],
  "http://example.com/term1": [{"@value": "v1"}],
  "http://example.com/term2": [{"@value": "v2", "@type": "http://example.com/t2"}],
  "http://example.com/term3": [{"@value": "v3", "@language": "en"}],
  "http://example.com/term4": [{"@value": 4}],
  "http://example.com/term5": [{"@value": 50}, {"@value": 51}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://example.com/vocab#name",
    "children": { "@reverse": "http://example.com/vocab#parent" },
    "pets": { "@reverse": "http://example.com/vocab#owner" }
  },
  "@id": "#homer",
  "name": "Homer",
  "children": [
    {
      "@id": "#bart",
      "name": "Bart"
    },
    {
      "@id": "#lisa",
      "name": "Lisa"
    }
  ],
  "pets": [
    {
      "@id": "#snowball-ii",
      "name": "Snowball II"
    },
    {
      "@id": "#santas-little-helper",
      "name": "Santa's Little Helper"
    }
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "https://w3c.github.io/json-ld-api/tests/expand/0078-in.jsonld#homer",
  "@reverse": {
    "http://example.com/vocab#parent": [
      {
        "@id": "https://w3c.github.io/json-ld-api/tests/expand/0078-in.jsonld#bart",
        "http://example.com/vocab#name": [ { "@value": "Bart" } ]
      },
      {
        "@id": "https://w3c.github.io/json-ld-api/tests/expand/0078-in.jsonld#lisa",
        "http://example.com/vocab#name": [ { "@value": "Lisa" } ]
      }
    ],
    "http://example.com/vocab#owner": [
      {
        "@id": "https://w3c.github.io/json-ld-api/tests/expand/0078-in.jsonld#snowball-ii",
        "http://example.com/vocab#name": [ { "@value": "Snowball II" } ]
      },
      {
        "@id": "https://w3c.github.io/json-ld-api/tests/expand/0078-in.jsonld#santas-little-helper",
        "http://example.com/vocab#name": [ { "@value": "Santa's Little Helper" } ]
      }
    ]
  },
  "http://example.com/vocab#name": [ { "@value": "Homer" } ]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "input": {"@id": "foo:input", "@container": "@graph"},
    "value": "foo:value"
  },
  "input": {
    "value": "x"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "foo:input": [{
    "@graph": [{
      "foo:value": [{
        "@value": "x"
      }]
    }]
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "input": {"@id": "foo:input", "@container": ["@graph", "@set"]},
    "value": "foo:value"
  },
  "input": [{
    "value": "x"
  }]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "foo:input": [{
    "@graph": [{
      "foo:value": [{
        "@value": "x"
      }]
    }]
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": "@graph"}
  },
  "input": {
    "@graph": {
      "value": "x"
    }
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@graph": [{
      "@graph": [{
        "http://example.org/value": [{"@value": "x"}]
      }]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@index"]}
  },
  "input": {
    "g1": {"value": "x"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@index": "g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@index", "@set"]}
  },
  "input": {
    "g1": {"value": "x"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@index": "g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@index"]}
  },
  "input": {
    "g1": {
      "@graph": {
        "value": "x"
      }
    }
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@index": "g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@id"]}
  },
  "input": {
    "http://example.com/g1": {"value": "x"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@id": "http://example.com/g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@id", "@set"]}
  },
  "input": {
    "http://example.com/g1": {"value": "x"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@id": "http://example.com/g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@id"]}
  },
  "input": {
    "http://example.com/g1": {
      "@graph": {
        "value": "x"
      }
    }
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@id": "http://example.com/g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "@base": "http://example.com/",
    "coerceId": {"@type": "@id"},
    "coerceVocab": {"@type": "@vocab"}
  },
  "coerceDefault": ["string", true, false, 0, 1],
  "coerceId": ["string", true, false, 0, 1],
  "coerceVocab": ["string", true, false, 0, 1]
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
 {
   "http://example.org/coerceDefault": [
     {"@value": "string"},
     {"@value": true},
     {"@value": false},
     {"@value": 0},
     {"@value": 1}
   ],
   "http://example.org/coerceId": [
     {"@id": "http://example.com/string"},
     {"@value": true},
     {"@value": false},
     {"@value": 0},
     {"@value": 1}
   ],
   "http://example.org/coerceVocab": [
     {"@id": "http://example.org/string"},
     {"@value": true},
     {"@value": false},
     {"@value": 0},
     {"@value": 1}
   ]
 }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@base": ""
  },
  "@id": "relative-iri",
  "http://prop": "value"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example/base/relative-iri",
  "http://prop": [{"@value": "value"}]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@base": ".."
  },
  "@id": "relative-iri",
  "http://prop": "value"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example/relative-iri",
  "http://prop": [{"@value": "value"}]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@base": "http://foo.bar/./baz/"
  }, {
    "@base": "example/"
  }],
  "@id": "relative-iri",
  "http://prop": "value"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://foo.bar/baz/example/relative-iri",
  "http://prop": [{"@value": "value"}]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@base": "http://example.com/some/deep/directory/and/file/",
    "@vocab": ""
  },
  "@id": "relativePropertyIris",
  "link": "link",
  "#fragment-works": "#fragment-works",
  "?query=works": "?query=works",
  "./": "./",
  "../": "../",
  "../parent": "../parent",
  "../../parent-parent-eq-root": "../../parent-parent-eq-root",
  "../../../../../still-root": "../../../../../still-root",
  "../.././.././../../too-many-dots": "../.././.././../../too-many-dots",
  "/absolute": "/absolute",
  "//example.org/scheme-relative": "//example.org/scheme-relative"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/some/deep/directory/and/file/relativePropertyIris",
    "http://example.com/some/deep/directory/and/file/#fragment-works": [{"@value": "#fragment-works"}],
    "http://example.com/some/deep/directory/and/file/../": [{"@value": "../"}],
    "http://example.com/some/deep/directory/and/file/../../../../../still-root": [{"@value": "../../../../../still-root"}],
    "http://example.com/some/deep/directory/and/file/../.././.././../../too-many-dots": [{"@value": "../.././.././../../too-many-dots"}],
    "http://example.com/some/deep/directory/and/file/../../parent-parent-eq-root": [{"@value": "../../parent-parent-eq-root"}],
    "http://example.com/some/deep/directory/and/file/../parent": [{"@value": "../parent"}],
    "http://example.com/some/deep/directory/and/file/./": [{"@value": "./"}],
    "http://example.com/some/deep/directory/and/file///example.org/scheme-relative": [{"@value": "//example.org/scheme-relative"}],
    "http://example.com/some/deep/directory/and/file//absolute": [{"@value": "/absolute"}],
    "http://example.com/some/deep/directory/and/file/?query=works": [{"@value": "?query=works"}],
    "http://example.com/some/deep/directory/and/file/link": [{"@value": "link"}]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "input": {"@id": "foo:input", "@container": "@graph"},
    "value": "foo:value"
  },
  "input": [{
    "value": "x"
  }, {
    "value": "y"
  }]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "foo:input": [{
    "@graph": [{
      "foo:value": [{
        "@value": "x"
      }]
    }]
  }, {
    "@graph": [{
      "foo:value": [{
        "@value": "y"
      }]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "input": {"@id": "foo:input", "@container": ["@graph", "@set"]},
    "value": "foo:value"
  },
  "input": [{
    "value": "x"
  }, {
    "value": "y"
  }]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "foo:input": [{
    "@graph": [{
      "foo:value": [{
        "@value": "x"
      }]
    }]
  }, {
    "@graph": [{
      "foo:value": [{
        "@value": "y"
      }]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": "@graph"}
  },
  "input": [{
    "@graph": {
      "value": "x"
    }
  }, {
    "@graph": {
      "value": "y"
    }
  }]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@graph": [{
      "@graph": [{"http://example.org/value": [{"@value": "x"}]}]
    }]
  }, {
    "@graph": [{
      "@graph": [{"http://example.org/value": [{"@value": "y"}]}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@index"]}
  },
  "input": {
    "g1": {"value": "x"},
    "g2": {"value": "y"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@index": "g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }, {
    "@index": "g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "y"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@index", "@set"]}
  },
  "input": {
    "g1": {"value": "x"},
    "g2": {"value": "y"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@index": "g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }, {
    "@index": "g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "y"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@index"]}
  },
  "input": {
    "g1": {"@graph": {"value": "x"}},
    "g2": {"@graph": {"value": "y"}}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@index": "g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }, {
    "@index": "g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "y"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@id"]}
  },
  "input": {
    "http://example.com/g1": {"value": "x"},
    "http://example.com/g2": {"value": "y"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@id": "http://example.com/g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }, {
    "@id": "http://example.com/g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "y"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@id", "@set"]}
  },
  "input": {
    "http://example.com/g1": {"value": "x"},
    "http://example.com/g2": {"value": "y"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@id": "http://example.com/g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }, {
    "@id": "http://example.com/g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "y"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@id"]}
  },
  "input": {
    "http://example.com/g1": {
      "@graph": {
        "value": "x"
      }
    },
    "http://example.com/g2": {
      "@graph": {
        "value": "y"
      }
    }
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@id": "http://example.com/g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }, {
    "@id": "http://example.com/g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "y"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": "@graph"}
  },
  "input": {
    "@graph": [{
      "value": "x"
    }, {
      "value": "y"
    }]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [
    {
      "@graph": [{
        "@graph": [{
          "http://example.org/value": [{"@value": "x"}]
        }, {
          "http://example.org/value": [{"@value": "y"}]
        }]
      }]
    }
  ]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": "@graph"}
  },
  "input": [{
    "@graph": {
      "value": "x"
    }
  }, {
    "@graph": {
      "value": "y"
    }
  }]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [
    {
      "@graph": [{
        "@graph": [{"http://example.org/value": [{"@value": "x"}]}]
      }]
    }, {
      "@graph": [{
        "@graph": [{"http://example.org/value": [{"@value": "y"}]}]
      }]
    }
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": "@graph"}
  },
  "input": [
    {"@graph": {"value": "x"}},
    {"value": "y"}
  ]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [
    {
      "@graph": [{
        "@graph": [{"http://example.org/value": [{"@value": "x"}]}]
      }]
    }, {
      "@graph": [{
          "http://example.org/value": [{"@value": "y"}]
      }]
    }
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@index"]}
  },
  "input": {
    "g1": [{"@graph": {"value": "x"}}, {"value": "y"}],
    "g2": [{"@graph": {"value": "a"}}, {"value": "b"}]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@index": "g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }, {
    "@index": "g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "y"}]
    }]
  }, {
    "@index": "g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "a"}]
    }]
  }, {
    "@index": "g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "b"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@id"]}
  },
  "input": {
    "http://example.com/g1": {
      "@graph": {
        "value": "x"
      }
    },
    "http://example.com/g2": {
      "@graph": {
        "value": "y"
      }
    }
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@id": "http://example.com/g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }, {
    "@id": "http://example.com/g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "y"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@index"]}
  },
  "input": {
    "g1": [{"value": "x"}, {"value": "y"}],
    "g2": [{"value": "a"}, {"value": "b"}]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@index": "g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }, {
    "@index": "g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "y"}]
    }]
  }, {
    "@index": "g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "a"}]
    }]
  }, {
    "@index": "g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "b"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@id"]}
  },
  "input": {
    "http://example.com/g1": [{"value": "x"}, {"value": "y"}],
    "http://example.com/g2": [{"value": "a"}, {"value": "b"}]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@id": "http://example.com/g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }, {
    "@id": "http://example.com/g1",
    "@graph": [{
      "http://example.org/value": [{"@value": "y"}]
    }]
  }, {
    "@id": "http://example.com/g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "a"}]
    }]
  }, {
    "@id": "http://example.com/g2",
    "@graph": [{
      "http://example.org/value": [{"@value": "b"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@base": "https://ex.org/",
    "u": {"@id": "urn:u:", "@type": "@id"}
  },
  "u": ["#Test", "#Test:2"]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "urn:u:": [
    {"@id": "https://ex.org/#Test"},
    {"@id": "https://ex.org/#Test:2"}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@base": "http://example.com/some/deep/directory/and/file/",
    "@vocab": "/relative"
  },
  "@id": "relativePropertyIris",
  "link": "link",
  "#fragment-works": "#fragment-works",
  "?query=works": "?query=works",
  "./": "./",
  "../": "../",
  "../parent": "../parent",
  "../../parent-parent-eq-root": "../../parent-parent-eq-root",
  "../../../../../still-root": "../../../../../still-root",
  "../.././.././../../too-many-dots": "../.././.././../../too-many-dots",
  "/absolute": "/absolute",
  "//example.org/scheme-relative": "//example.org/scheme-relative"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/some/deep/directory/and/file/relativePropertyIris",
    "http://example.com/relative#fragment-works": [{"@value": "#fragment-works"}],
    "http://example.com/relative../": [{"@value": "../"}],
    "http://example.com/relative../../../../../still-root": [{"@value": "../../../../../still-root"}],
    "http://example.com/relative../.././.././../../too-many-dots": [{"@value": "../.././.././../../too-many-dots"}],
    "http://example.com/relative../../parent-parent-eq-root": [{"@value": "../../parent-parent-eq-root"}],
    "http://example.com/relative../parent": [{"@value": "../parent"}],
    "http://example.com/relative./": [{"@value": "./"}],
    "http://example.com/relative//example.org/scheme-relative": [{"@value": "//example.org/scheme-relative"}],
    "http://example.com/relative/absolute": [{"@value": "/absolute"}],
    "http://example.com/relative?query=works": [{"@value": "?query=works"}],
    "http://example.com/relativelink": [{"@value": "link"}]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@version": 1.1,
    "@base": "http://example.com/some/deep/directory/and/file/",
    "@vocab": "http://example.com/vocabulary/"
  }, {
    "@vocab": "./rel2#"
  }],
  "@id": "relativePropertyIris",
  "link": "link",
  "#fragment-works": "#fragment-works",
  "?query=works": "?query=works",
  "./": "./",
  "../": "../",
  "../parent": "../parent",
  "../../parent-parent-eq-root": "../../parent-parent-eq-root",
  "../../../../../still-root": "../../../../../still-root",
  "../.././.././../../too-many-dots": "../.././.././../../too-many-dots",
  "/absolute": "/absolute",
  "//example.org/scheme-relative": "//example.org/scheme-relative"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/some/deep/directory/and/file/relativePropertyIris",
    "http://example.com/vocabulary/./rel2##fragment-works": [{"@value": "#fragment-works"}],
    "http://example.com/vocabulary/./rel2#../": [{"@value": "../"}],
    "http://example.com/vocabulary/./rel2#../../../../../still-root": [{"@value": "../../../../../still-root"}],
    "http://example.com/vocabulary/./rel2#../.././.././../../too-many-dots": [{"@value": "../.././.././../../too-many-dots"}],
    "http://example.com/vocabulary/./rel2#../../parent-parent-eq-root": [{"@value": "../../parent-parent-eq-root"}],
    "http://example.com/vocabulary/./rel2#../parent": [{"@value": "../parent"}],
    "http://example.com/vocabulary/./rel2#./": [{"@value": "./"}],
    "http://example.com/vocabulary/./rel2#//example.org/scheme-relative": [{"@value": "//example.org/scheme-relative"}],
    "http://example.com/vocabulary/./rel2#/absolute": [{"@value": "/absolute"}],
    "http://example.com/vocabulary/./rel2#?query=works": [{"@value": "?query=works"}],
    "http://example.com/vocabulary/./rel2#link": [{"@value": "link"}]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@version": 1.1,
    "@base": "http://example.com/some/deep/directory/and/file/",
    "@vocab": "/rel1"
  }, {
    "@vocab": "./rel2#"
  }],
  "@id": "relativePropertyIris",
  "link": "link",
  "#fragment-works": "#fragment-works",
  "?query=works": "?query=works",
  "./": "./",
  "../": "../",
  "../parent": "../parent",
  "../../parent-parent-eq-root": "../../parent-parent-eq-root",
  "../../../../../still-root": "../../../../../still-root",
  "../.././.././../../too-many-dots": "../.././.././../../too-many-dots",
  "/absolute": "/absolute",
  "//example.org/scheme-relative": "//example.org/scheme-relative"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/some/deep/directory/and/file/relativePropertyIris",
    "http://example.com/rel1./rel2##fragment-works": [{"@value": "#fragment-works"}],
    "http://example.com/rel1./rel2#../": [{"@value": "../"}],
    "http://example.com/rel1./rel2#../../../../../still-root": [{"@value": "../../../../../still-root"}],
    "http://example.com/rel1./rel2#../.././.././../../too-many-dots": [{"@value": "../.././.././../../too-many-dots"}],
    "http://example.com/rel1./rel2#../../parent-parent-eq-root": [{"@value": "../../parent-parent-eq-root"}],
    "http://example.com/rel1./rel2#../parent": [{"@value": "../parent"}],
    "http://example.com/rel1./rel2#./": [{"@value": "./"}],
    "http://example.com/rel1./rel2#//example.org/scheme-relative": [{"@value": "//example.org/scheme-relative"}],
    "http://example.com/rel1./rel2#/absolute": [{"@value": "/absolute"}],
    "http://example.com/rel1./rel2#?query=works": [{"@value": "?query=works"}],
    "http://example.com/rel1./rel2#link": [{"@value": "link"}]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "valueOf": "http://example.org/valueOf",
    "toString": "http://example.org/toString"
  },
  "valueOf": "first",
  "toString": "second"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/toString": [
      {
        "@value": "second"
      }
    ],
    "http://example.org/valueOf": [
      {
        "@value": "first"
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "type1": "@type",
    "type2": "@type"
  },
  "type1": "Type1",
  "type2": "Type2"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["http://example.org/Type1", "http://example.org/Type2"]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@base": "http://example.com/some/deep/directory/and/file/",
    "@vocab": ""
  },
  "@id": "relativePropertyIris",
  "link": "link"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid vocab mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": {
    "@base": "http://example.com/some/deep/directory/and/file/",
    "@vocab": "/relative"
  },
  "@id": "relativePropertyIris",
  "link": "link"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid vocab mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/vocab",
    ":term": {"@type": "@id"}
  },
  ":term": "http://example.org/base"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab:term": [{"@id": "http://example.org/base"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"@vocab": "http://schema.org/"},
  "@id": "foo:bar-id",
  "@type": "foo:bar-type",
  "foo:bar": "is an absolute iri property",
  "term": "is schema.org/term",
  ":fish": "is schema.org/:fish"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "foo:bar-id",
  "@type": ["foo:bar-type"],
  "foo:bar": [{"@value": "is an absolute iri property"}],
  "http://schema.org/term": [{"@value": "is schema.org/term"}],
  "http://schema.org/:fish": [{"@value": "is schema.org/:fish"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@": "http://example.org/vocab/at",
    "@foo.bar": "http://example.org/vocab/foo.bar",
    "@ignoreMe": "http://example.org/vocab/ignoreMe"
  },
  "@": "allowed",
  "@foo.bar": "allowed",
  "@ignoreMe": "ignored"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab/at": [{"@value": "allowed"}],
  "http://example.org/vocab/foo.bar": [{"@value": "allowed"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.orb/vocab/",
    "at": {"@id": "@"},
    "foo.bar": {"@id": "@foo.bar"},
    "ignoreMe": {"@id": "@ignoreMe"}
  },
  "at": "allowed",
  "foo.bar": "allowed",
  "ignoreMe": "resolves to @vocab"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.orb/vocab/@": [{"@value": "allowed"}],
  "http://example.orb/vocab/@foo.bar": [{"@value": "allowed"}],
  "http://example.orb/vocab/ignoreMe": [{"@value": "resolves to @vocab"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@base": "http://example.org/",
    "@vocab": "http://example.org/vocab/",
    "at": {"@reverse": "@"},
    "foo.bar": {"@reverse": "@foo.bar"}
  },
  "@id": "foo",
  "at": {"@id": "allowed"},
  "foo.bar": {"@id": "allowed"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.org/foo",
  "@reverse": {
    "http://example.org/vocab/@": [{"@id": "http://example.org/allowed"}],
    "http://example.org/vocab/@foo.bar": [{"@id": "http://example.org/allowed"}]
  }
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@base": "http://example.org/"
  },
  "http://example.org/vocab/at": {"@id": "@"},
  "http://example.org/vocab/foo.bar": {"@id": "@foo.bar"},
  "http://example.org/vocab/ignoreme": {"@id": "@ignoreMe"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab/at": [{"@id": "http://example.org/@"}],
  "http://example.org/vocab/foo.bar": [{"@id": "http://example.org/@foo.bar"}],
  "http://example.org/vocab/ignoreme": [{"@id": null}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@id": "http://example.com/foo",
  "http://example.com/bar": {"@value": "bar", "@type": "http://example.com/baz z"}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid typed value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    {
      "@version": 1.1,
      "ex": {
        "@id": "http://example.org/",
        "@prefix": true
      }
    },
    {
      "@vocab": "ex:ns/"
    }
  ],
  "foo": "bar"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/ns/foo": [
      {
        "@value": "bar"
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    {
      "@version": 1.1,
      "ex": {
        "@id": "http://example.org/",
        "@prefix": true
      }
    },
    {
      "@vocab": "ex"
    }
  ],
  "foo": "bar"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/foo": [
      {
        "@value": "bar"
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": "0126-context.jsonld",
  "@id": "ex:id",
  "prop": {
    "value": "v"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "ex:id",
    "ex:prop": [
      {
        "ex:value": [
          {
            "@value": "v"
          }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": "0127-context-1.jsonld",
  "@id": "ex:id",
  "prop": {
    "value": "v"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "ex:id",
    "ex:prop": [
      {
        "ex:value": [
          {
            "@value": "v"
          }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    "0128-context-1.jsonld",
    "0128-context-2.jsonld"
  ],
  "@id": "ex:id",
  "prop": {
    "value": "v"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "ex:id",
    "ex:prop": [
      {
        "ex:value": [
          {
            "@value": "v"
          }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context" : {"@base": "http://example"},
  "@id": "relative-iri",
  "http://prop": "value"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example/relative-iri",
  "http://prop": [{"@value": "value"}]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context" : {"@base": "http://example/base"},
  "@id": "relative-iri",
  "http://prop": "value"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example/relative-iri",
  "http://prop": [{"@value": "value"}]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@base": "https://example.org/",
    "@vocab": "https://example.net/ns#",
    "rdf": "http://www.w3.org/1999/02/22-rdf-syntax-ns#",
    "statement": {"@reverse": "rdf:subject", "@container": "@index", "@index": "predicate"},
    "predicate": {"@id": "rdf:predicate", "@type": "@vocab"},
    "term": {"@id": "rdf:object", "@type": "@vocab"},
    "addedIn": {"@type": "@id"}
  },
  "@id": "item/1",
  "statement": {
    "rdf:type": {"term": "A", "addedIn": "v1"}
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "https://example.org/item/1",
    "@reverse": {
      "http://www.w3.org/1999/02/22-rdf-syntax-ns#subject": [
        {
          "https://example.net/ns#addedIn": [
            {
              "@id": "https://example.org/v1"
            }
          ],
          "http://www.w3.org/1999/02/22-rdf-syntax-ns#object": [
            {
              "@id": "https://example.net/ns#A"
            }
          ],
          "http://www.w3.org/1999/02/22-rdf-syntax-ns#predicate": [
            {
              "@id": "http://www.w3.org/1999/02/22-rdf-syntax-ns#type"
            }
          ]
        }
      ]
    }
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "foo": {"@context": {"bar": "http://example.org/bar"}}
  },
  "foo": {
    "bar": "baz"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example/foo": [{"http://example.org/bar": [{"@value": "baz"}]}]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "foo": {"@context": {"bar": {"@type": "@id"}}},
    "bar": {"@type": "http://www.w3.org/2001/XMLSchema#string"}
  },
  "foo": {
    "bar": "http://example/baz"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example/foo": [{"http://example/bar": [{"@id": "http://example/baz"}]}]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "foo": {"@context": {"Bar": {"@id": "bar"}}}
  },
  "foo": {
    "Bar": "baz"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example/foo": [{
      "http://example/bar": [
        {"@value": "baz"}
      ]}
    ]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "foo": {"@context": {"baz": {"@type": "@vocab"}}}
  },
  "foo": {
    "bar": {
      "baz": "buzz"
    }
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example/foo": [{
      "http://example/bar": [{
        "http://example/baz": [{"@id": "http://example/buzz"}]
      }]
    }]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "b": {"@context": {"c": "http://example.org/c"}}
  },
  "a": {
    "@context": {"@vocab": "http://example.com/"},
    "b": {
      "a": "A in example.com",
      "c": "C in example.org"
    },
    "c": "C in example.com"
  },
  "c": "C in example"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/a": [{
    "http://example.com/c": [{"@value": "C in example.com"}],
    "http://example/b": [{
      "http://example.com/a": [{"@value": "A in example.com"}],
      "http://example.org/c": [{"@value": "C in example.org"}]
    }]
  }],
  "http://example/c": [{"@value": "C in example"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "Foo": {"@context": {"bar": "http://example.org/bar"}}
  },
  "a": {"@type": "Foo", "bar": "baz"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example/a": [{
      "@type": ["http://example/Foo"],
      "http://example.org/bar": [{"@value": "baz"}]
    }]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "Foo": {"@context": {"bar": {"@type": "@id"}}},
    "bar": {"@type": "http://www.w3.org/2001/XMLSchema#string"}
  },
  "a": {"@type": "Foo", "bar": "http://example/baz"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example/a": [{
      "@type": ["http://example/Foo"],
      "http://example/bar": [{"@id": "http://example/baz"}]
    }]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "type": "@type",
    "Foo": {"@context": {"bar": "http://example.org/bar"}}
  },
  "a": {"type": "Foo", "bar": "baz"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example/a": [{
      "@type": ["http://example/Foo"],
      "http://example.org/bar": [{"@value": "baz"}]
    }]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "Foo": {"@context": {"baz": {"@type": "@vocab"}}}
  },
  "@type": "Foo",
  "bar": {"baz": "buzz"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@type": ["http://example/Foo"],
    "http://example/bar": [{
      "http://example/baz": [{"@value": "buzz"}]
    }]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "B": {"@context": {"c": "http://example.org/c"}}
  },
  "a": {
    "@context": {"@vocab": "http://example.com/"},
    "@type": "B",
    "a": "A in example.com",
    "c": "C in example.org"
  },
  "c": "C in example"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/a": [{
    "@type": ["http://example/B"],
    "http://example.com/a": [{"@value": "A in example.com"}],
    "http://example.org/c": [{"@value": "C in example.org"}]
  }],
  "http://example/c": [{"@value": "C in example"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "t1": {"@context": {"foo": {"@id": "http://example.com/foo"}}},
    "t2": {"@context": {"foo": {"@id": "http://example.org/foo", "@type": "@id"}}}
  },
  "@type": ["t2", "t1"],
  "foo": "urn:bar"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["http://example/t2", "http://example/t1"],
  "http://example.org/foo": [
    {"@id": "urn:bar"}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "Foo": {
      "@context": {
        "bar": {
          "@context": {
            "baz": {"@type": "@vocab"}
          }
        }
      }
    }
  },
  "@type": "Foo",
  "bar": {"baz": "buzz"}
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@type": ["http://example/Foo"],
    "http://example/bar": [{
      "http://example/baz": [{"@id": "http://example/buzz"}]
    }]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "prop": {"@container": "@index"},
    "foo": "http://example/base-foo",
    "Outer": {
      "@context": {
        "prop": {
          "@id": "http://example/outer-prop",
          "@container": "@type"
        }
      }
    },
    "Inner": {"@context": {"foo": "http://example/inner-foo"}}
  },
  "@type": "Outer",
  "prop": {
    "Inner": {
      "prop": {
        "index": {
          "@id": "http://example/inner-with-index",
          "foo": "inner-foo"
        }
      }
    }
  },
  "foo": "base-foo"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
 "@type": ["http://example/Outer"],
 "http://example/base-foo": [{"@value": "base-foo"}],
 "http://example/outer-prop": [{
   "@type": ["http://example/Inner"],
   "http://example/prop": [{
     "@id": "http://example/inner-with-index",
     "@index": "index",
     "http://example/inner-foo": [{"@value": "inner-foo"}]
   }]
 }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "foo": "http://example/foo",
    "Type": {
      "@context": [
        null
      ]
    }
  },
  "foo": "will-exist",
  "p": {
    "@type": "Type",
    "foo": "will-not-exist"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/foo": [{
    "@value": "will-exist"
  }],
  "http://example/p": [{
    "@type": ["http://example/Type"]
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@base": "http://example/base-base",
    "@vocab": "http://example/",
    "foo": "http://example/foo",
    "Type": {
      "@context": {
        "@base": "http://example/typed-base"
      }
    }
  },
  "@id": "#base-id",
  "p": {
    "@id": "#typed-id",
    "@type": "Type",
    "subjectReference": {
      "@id": "#subject-reference-id"
    },
    "nestedNode": {
      "@id": "#nested-id",
      "foo": "bar"
    }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example/base-base#base-id",
  "http://example/p": [{
    "@id": "http://example/typed-base#typed-id",
    "@type": ["http://example/Type"],
    "http://example/subjectReference": [{
      "@id": "http://example/typed-base#subject-reference-id"
    }],
    "http://example/nestedNode": [{
      "@id": "http://example/base-base#nested-id",
      "http://example/foo": [{
        "@value": "bar"
      }]
    }]
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "Type": {
      "@context": {
        "@vocab": "http://example.com/"
      }
    }
  },
  "foo": "org",
  "p": {
    "@type": "Type",
    "foo": "com",
    "nested": {
      "nested-prop": "org"
    }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/foo": [{
    "@value": "org"
  }],
  "http://example.org/p": [{
    "@type": ["http://example.org/Type"],
    "http://example.com/foo": [{
      "@value": "com"
    }],
    "http://example.com/nested": [{
      "http://example.org/nested-prop": [{
        "@value": "org"
      }]
    }]
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "Bar": {
      "@context": [
        {
          "prop": "http://example/bar-prop"
        }
      ]
    },
    "Foo": {
      "@context": [
        {
          "prop": "http://example/foo-prop"
        }
      ]
    }
  },
  "@type": ["Foo", "Bar"],
  "prop": "foo",
  "nested": {
    "prop": "vocab"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": [
    "http://example/Foo",
    "http://example/Bar"
  ],
  "http://example/foo-prop": [{
    "@value": "foo"
  }],
  "http://example/nested": [{
    "http://example/prop": [{
      "@value": "vocab"
    }]
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "Bar": {
      "@context": [
        null,
        {
          "prop": "http://example/bar-prop"
        }
      ]
    },
    "Foo": {
      "@context": [
        null,
        {
          "prop": "http://example/foo-prop"
        }
      ]
    }
  },
  "@type": ["Foo", "Bar"],
  "prop": "foo",
  "nested": {
    "prop": "will-not-exist"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": [
    "http://example/Foo",
    "http://example/Bar"
  ],
  "http://example/foo-prop": [
    {
      "@value": "foo"
    }
  ]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "prop": "http://example/base-prop",
    "Type": {
      "@context": {
        "foo": {
          "@context": {
            "prop": "http://example/foo-prop"
          }
        },
        "bar": {
          "@context": {
            "prop": "http://example/bar-prop"
          }
        }
      }
    }
  },
  "@type": "Type",
  "foo": {
    "prop": "foo"
  },
  "bar": {
    "prop": "bar"
  },
  "baz": {
    "prop": "baz"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": [
    "http://example/Type"
  ],
  "http://example/foo": [{
    "http://example/foo-prop": [
      {
        "@value": "foo"
      }
    ]
  }],
  "http://example/bar": [{
    "http://example/bar-prop": [
      {
        "@value": "bar"
      }
    ]
  }],
  "http://example/baz": [{
    "http://example/base-prop": [
      {
        "@value": "baz"
      }
    ]
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "type": "@type",
    "Type": {
      "@context": {
        "value": "@value"
      }
    }
  },
  "type": "Type",
  "v": {
    "value": "value",
    "type": "value-type"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["http://example/Type"],
  "http://example/v": [{
    "@type": "http://example/value-type",
    "@value": "value"
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "ex:",
    "type": "@type",
    "prop": "ex:untyped",
    "Type": {
      "@context": {
        "prop": "ex:typed",
        "value": "@value"
      }
    }
  },
  "prop": {
    "type": "Type",
    "prop": [
      "v1",
      {
        "value": "v2"
      },
      {
        "@value": "v3"
      },
      {
        "prop": [
          "v4",
          {
            "type": "Type",
            "prop": "v5"
          }
        ]
      }
    ]
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "ex:untyped": [{
    "@type": ["ex:Type"],
    "ex:typed": [{
      "@value": "v1"
    }, {
      "@value": "v2"
    }, {
      "@value": "v3"
    }, {
      "ex:untyped": [{
        "@value": "v4"
      }, {
        "@type": ["ex:Type"],
        "ex:typed": [{"@value": "v5"}]
      }]
    }]
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "ex:",
    "Type": {
      "@context": {
        "foo": {
          "@id": "ex:foo",
          "@type": "@vocab",
          "@context": {
            "@version": 1.1,
            "Foo": "ex:Foo",
            "Bar": "ex:Bar"
          }
        }
      }
    }
  },
  "@type": "Type",
  "foo": "Bar"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["ex:Type"],
  "ex:foo": [{"@id": "ex:Bar"}]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "Outer": {
      "@id": "ex:Outer",
      "@context": {
        "nested": "ex:nested"
      }
    },
    "Inner": {
      "@id": "ex:Inner",
      "@context": {
        "@version": 1.1,
        "foo": {
          "@id": "ex:foo",
          "@type": "@vocab",
          "@context": {
            "Foo": "ex:Foo"
          }
        }
      }
    }
  },
  "@type": "Outer",
  "nested": {
    "@type": "Inner",
    "foo": "Foo"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["ex:Outer"],
  "ex:nested": [{
    "@type": ["ex:Inner"],
    "ex:foo": [{"@id": "ex:Foo"}]
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "Outer": {
      "@id": "ex:Outer",
      "@context": {
        "nested": "ex:nested"
      }
    },
    "Inner": {
      "@id": "ex:Inner",
      "@context": {
        "@version": 1.1,
        "foo": {
          "@id": "ex:foo",
          "@container": "@set",
          "@type": "ex:Number",
          "@context": {
            "value": "@value"
          }
        },
        "bar": {
          "@id": "ex:bar",
          "@container": "@set",
          "@type": "@id",
          "@context": {
            "@base": "http://example/"
          }
        }
      }
    }
  },
  "@type": "Outer",
  "nested": {
    "@type": "Inner",
    "foo": [{"value": "1"}, "2"],
    "bar": [{"@id": "a"}, "b"]
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["ex:Outer"],
  "ex:nested": [{
    "@type": ["ex:Inner"],
    "ex:foo": [
      {"@value": "1"},
      {"@type": "ex:Number", "@value": "2"}
    ],
    "ex:bar": [
      {"@id": "http://example/a"},
      {"@id": "http://example/b"}
    ]
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "type": "@type",
    "Outer": {
      "@id": "ex:Outer",
      "@context": {
        "nested": {
          "@id": "ex:nested",
          "@type": "@id",
          "@container": "@graph"
        }
      }
    },
    "Inner": {
      "@id": "ex:Inner",
      "@context": {
        "foo": "ex:foo"
      }
    }
  },
  "type": "Outer",
  "nested": {
    "type": "Inner",
    "foo": "bar"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["ex:Outer"],
  "ex:nested": [{
    "@graph": [{
      "@type": ["ex:Inner"],
      "ex:foo": [{"@value": "bar"}]
    }]
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example/",
    "Foo": {
      "@context": {
        "@propagate": true,
        "baz": {"@type": "@vocab"}
      }
    }
  },
  "@type": "Foo",
  "bar": {"baz": "buzz"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@type": ["http://example/Foo"],
    "http://example/bar": [{
      "http://example/baz": [{"@id": "http://example/buzz"}]
    }]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example/",
    "bar": {
      "@context": {
        "@propagate": false,
        "baz": {"@type": "@id"}
      }
    }
  },
  "bar": {"baz": {"baz": "buzz"}}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
 "http://example/bar": [{
   "http://example/baz": [{
     "http://example/baz": [{"@value": "buzz"}]
   }]
 }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example/"
  },
  "bar": {
    "@context": {
      "@propagate": false,
      "baz": {"@type": "@vocab"}
    },
    "baz": {
      "baz": "buzz"
    }
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
 "http://example/bar": [{
   "http://example/baz": [{
     "http://example/baz": [{"@value": "buzz"}]
   }]
 }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@propagate": true
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid context entry)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@propagate": "not boolean"
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @propagate value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    {"@base": "http://example.org/a/"},
    "c031/c031-context.jsonld"
  ],
  "outer": {
    "inner": "ab"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://a.example/out": [{
      "http://a.example/in": [{
        "@value":"ab"
      }]
    }]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "t1": {
      "@id": "ex:t1",
      "@context": {
        "t2": {
          "@context": {"type": null}
        }
      }
    }
  },
  "t1": "something"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid scoped context)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "t1": {
      "@id": "ex:t1",
      "@context": {
        "t2": {
          "@context": {"type": null}
        }
      }
    }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid scoped context)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "foo": {"@context": "c034-context.jsonld"}
  },
  "foo": {
    "bar": "baz"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example/foo": [{"http://example.org/bar": [{"@value": "baz"}]}]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://vocab.org/",
    "prop1": {}
  },
  "@id": "ex:outer",
  "foo": {
    "@context": {
      "@vocab": "http://vocab.override.org/"
    },
    "@id": "ex:inner",
    "prop1": "baz1",
    "prop2": "baz2"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "ex:outer",
    "http://vocab.org/foo": [
      {
        "@id": "ex:inner",
        "http://vocab.org/prop1": [
          {
            "@value": "baz1"
          }
        ],
        "http://vocab.override.org/prop2": [
          {
            "@value": "baz2"
          }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "thing": {
      "@id": "ex:thing",
      "@context": {}
    },
    "title": "ex:title"
  },
  "title": "top",
  "thing": {
    "title": "sub"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "ex:title": [{"@value": "top"}
  ],
  "ex:thing": [{
    "ex:title": [{"@value": "sub"}]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/",
    "nest": {
      "@id": "@nest",
      "@context": {
        "@vocab": "http://example.org/nest/"
      }
    }
  },
  "nest": {
    "property": "should be in /nest"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/nest/property": [{"@value": "should be in /nest"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@base": "http://example.org/base/",
    "@vocab": "http://purl.org/dc/terms/",
    "bibo": "http://purl.org/ontology/bibo/",
    "Print": "bibo:Book",
    "name": "http://www.w3.org/2000/01/rdf-schema#label",
    "instanceOf": "@nest",
    "contributionByRole": {
      "@id": "@nest",
      "@context": {
        "agent": "@nest",
        "aut": "creator"
      }
    },
    "provisionActivityByType": {
      "@id": "@nest",
      "@context": {
        "Publication": {
          "@id": "@nest",
          "@context": {"date": "published", "agent": "publisher"}
        }
      }
    },
    "identifiedByType": {
      "@id": "@nest",
      "@context": {
        "Isbn": {"@id": "@nest"},
        "value": "bibo:isbn"
      }
    }
  },
  "@id": "book/one",
  "@type": "Print",
  "instanceOf": {
    "contributionByRole": {
      "aut": {
        "agent": {"name": "Some Body"}
      }
    }
  },
  "identifiedByType": {
    "Isbn": {
      "value": "1234567890"
    }
  },
  "provisionActivityByType": {
    "Publication": {
      "date": "1999",
      "agent": {"name": "PubCorp"}
    }
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.org/base/book/one",
  "@type": ["http://purl.org/ontology/bibo/Book"],
  "http://purl.org/dc/terms/creator": [{
    "http://www.w3.org/2000/01/rdf-schema#label": [{"@value": "Some Body"}]
  }],
  "http://purl.org/ontology/bibo/isbn": [{"@value": "1234567890"}],
  "http://purl.org/dc/terms/published": [{"@value": "1999"}],
  "http://purl.org/dc/terms/publisher": [{
    "http://www.w3.org/2000/01/rdf-schema#label": [{"@value": "PubCorp"}]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@direction": "rtl",
    "ex": "http://example.org/vocab#",
    "ex:ltr": { "@direction": "ltr" },
    "ex:none": { "@direction": null }
  },
  "ex:rtl": "rtl",
  "ex:ltr": "ltr",
  "ex:none": "no direction"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/vocab#rtl": [{"@value": "rtl", "@direction": "rtl"}],
    "http://example.org/vocab#ltr": [{"@value": "ltr", "@direction": "ltr"}],
    "http://example.org/vocab#none": [{"@value": "no direction"}]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@language": "en",
    "@direction": "rtl",
    "ex": "http://example.org/vocab#",
    "ex:ltr": { "@direction": "ltr" },
    "ex:none": { "@direction": null },
    "ex:german": { "@language": "de" },
    "ex:nolang": { "@language": null },
    "ex:german_ltr": { "@language": "de", "@direction": "ltr" },
    "ex:nolang_ltr": { "@language": null, "@direction": "ltr" },
    "ex:none_none": { "@language": null, "@direction": null },
    "ex:german_none": { "@language": "de", "@direction": null }
  },
  "ex:rtl": "rtl en",
  "ex:ltr": "ltr en",
  "ex:none": "no direction en",
  "ex:german": "german rtl",
  "ex:nolang": "no language rtl",
  "ex:german_ltr": "german ltr",
  "ex:nolang_ltr": "no language ltr",
  "ex:none_none": "no language or direction",
  "ex:german_none": "german no direction"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/vocab#rtl": [{"@value": "rtl en", "@language": "en", "@direction": "rtl"}],
    "http://example.org/vocab#ltr": [{"@value": "ltr en", "@language": "en", "@direction": "ltr"}],
    "http://example.org/vocab#none": [{"@value": "no direction en", "@language": "en"}],
    "http://example.org/vocab#german": [{"@value": "german rtl", "@language": "de", "@direction": "rtl"}],
    "http://example.org/vocab#nolang": [{"@value": "no language rtl", "@direction": "rtl"}],
    "http://example.org/vocab#german_ltr": [{"@value": "german ltr", "@language": "de", "@direction": "ltr"}],
    "http://example.org/vocab#nolang_ltr": [{"@value": "no language ltr", "@direction": "ltr"}],
    "http://example.org/vocab#none_none": [{"@value": "no language or direction"}],
    "http://example.org/vocab#german_none": [{"@value": "german no direction", "@language": "de"}]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "foo_ltr": {"@id": "http://example.com/foo", "@container": "@list", "@direction": "ltr"},
    "foo_rtl": {"@id": "http://example.com/foo", "@container": "@list", "@direction": "rtl"}
  },
  "foo_ltr": ["en"],
  "foo_rtl": ["ar"]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/foo": [
    {"@list": [{"@value": "en", "@direction": "ltr"}]},
    {"@list": [{"@value": "ar", "@direction": "rtl"}]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@direction": "ltr",
    "vocab": "http://example.com/vocab/",
    "label": {
      "@id": "vocab:label",
      "@container": "@language"
    }
  },
  "@id": "http://example.com/queen",
  "label": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/queen",
    "http://example.com/vocab/label": [
      {"@value": "Die Königin", "@language": "de", "@direction": "ltr"},
      {"@value": "Ihre Majestät", "@language": "de", "@direction": "ltr"},
      {"@value": "The Queen", "@language": "en", "@direction": "ltr"}
    ]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "vocab": "http://example.com/vocab/",
    "label": {
      "@id": "vocab:label",
      "@direction": "ltr",
      "@container": "@language"
    }
  },
  "@id": "http://example.com/queen",
  "label": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/queen",
    "http://example.com/vocab/label": [
      {"@value": "Die Königin", "@language": "de", "@direction": "ltr"},
      {"@value": "Ihre Majestät", "@language": "de", "@direction": "ltr"},
      {"@value": "The Queen", "@language": "en", "@direction": "ltr"}
    ]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "vocab": "http://example.com/vocab/",
    "@direction": "rtl",
    "label": {
      "@id": "vocab:label",
      "@direction": "ltr",
      "@container": "@language"
    }
  },
  "@id": "http://example.com/queen",
  "label": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/queen",
    "http://example.com/vocab/label": [
      {"@value": "Die Königin", "@language": "de", "@direction": "ltr"},
      {"@value": "Ihre Majestät", "@language": "de", "@direction": "ltr"},
      {"@value": "The Queen", "@language": "en", "@direction": "ltr"}
    ]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "vocab": "http://example.com/vocab/",
    "@direction": "rtl",
    "label": {
      "@id": "vocab:label",
      "@direction": null,
      "@container": "@language"
    }
  },
  "@id": "http://example.com/queen",
  "label": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/queen",
    "http://example.com/vocab/label": [
      {"@value": "Die Königin", "@language": "de"},
      {"@value": "Ihre Majestät", "@language": "de"},
      {"@value": "The Queen", "@language": "en"}
    ]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@direction": "foo"
  },
  "vocab:term": "bar"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid base direction)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "ex:p": {
    "@value": "v",
    "@type": "ex:t",
    "@direction": "rtl"
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid value object)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example/", "@index": true}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@type": {}
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Keyword redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example/term", "@container": "@context"}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid container mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"@vocab": "http://example.org/"},
  "@nest": "This should generate an error"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @nest value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"@vocab": "http://example.org/"},
  "@nest": true
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @nest value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"@vocab": "http://example.org/"},
  "@nest": 1
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @nest value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"@vocab": "http://example.org/"},
  "@nest": {"@value": "This should generate an error"}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @nest value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example/term", "@nest": "@id"}
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @nest value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@reverse": "http://example/term", "@nest": "@nest"}
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid reverse property)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Processing mode conflict)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.0
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @version value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@type": "@id"
  },
  "@type": "http://example.org/type"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Keyword redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": "er02-in.jsonld",
  "@id": "http://example/test#example"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Recursive context inclusion)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": "er03-in.jsonld",
  "@id": "http://example/test#example"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Recursive context inclusion)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": "tag:non-dereferencable-iri",
  "@id": "http://example/test#example"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Loading remote context failed)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld([{
  "@context": "er05-in.jsonld",
  "@id": "http://example/test#example"
}]
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid remote context)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": true,
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid local context)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"@base": true},
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid base IRI)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"@vocab": true},
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid vocab mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"@language": true},
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid default language)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "term:term"}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Cyclic IRI mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": true
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example/term", "@type": true}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid type mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example/term", "@type": "_:not-an-iri"}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid type mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example/term", "@reverse": "http://example/reverse"}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid reverse property)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@reverse": true}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid IRI mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@reverse": "http://example/reverse", "@container": "@list"}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid reverse property)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": true}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid IRI mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "@context"}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid keyword alias)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@container": "@set"}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid IRI mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example/term", "@container": "@id"}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid container mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example/term", "@language": true}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid language mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example/term", "@type": "relative/iri"}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid type mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"foo": {"@id": "http://example.com/foo", "@container": "@list"}},
  "foo": [{"@list": ["baz"]}]
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(List of lists)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@id": "http://example/foo",
  "@reverse": {
    "@id": "http://example/bar"
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid reverse property map)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "id": "@id",
    "ID": "@id"
  },
  "id": "http://example/foo",
  "ID": "http://example/bar"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Colliding keywords)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@id": true
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @id value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@type": true
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid type value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example/prop": {"@value": ["foo"]}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid value object value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example/prop": {"@value": "foo", "@language": true}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid language-tagged string)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example.com/vocab/indexMap": {
    "@value": "simple string",
    "@language": "en",
    "@index": true
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @index value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example.com/foo": {"@list": [{"@list": ["baz"]}]}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(List of lists)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "http://example/prop": {
    "@reverse": true
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @reverse value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "name": "http://xmlns.com/foaf/0.1/name"
  },
  "@id": "http://example.com/people/markus",
  "name": "Markus Lanthaler",
  "@reverse": {
    "http://xmlns.com/foaf/0.1/knows": "Dave Longley"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid reverse property value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "vocab": "http://example.com/vocab/",
    "label": {
      "@id": "vocab:label",
      "@container": "@language"
    }
  },
  "@id": "http://example.com/queen",
  "label": {
    "en": true
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid language map value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@reverse": "http://example/reverse"}
  },
  "@id": "http://example/foo",
  "term": {"@list": ["http://example/bar"]}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid reverse property value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example/foo": {"@value": "bar", "@id": "http://example/baz"}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid value object)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example/foo": {"@value": "bar", "@language": "en", "@type": "http://example/type"}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid value object)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example/foo": {"@value": true, "@language": "en"}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid language-tagged value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example/foo": {"@value": "bar", "@type": "_:dt"}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid typed value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example/prop": {"@list": ["foo"], "@id": "http://example/bar"}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid set or list object)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@type": {"@container": "@set"}
  },
  "@type": "http://example.org/type"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Keyword redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": {
    "http://www.w3.org/1999/02/22-rdf-syntax-ns#type": {"@id": "@type", "@type": "@id"}
  },
  "@graph": [
    {
      "@id": "http://example.com/a",
      "http://www.w3.org/1999/02/22-rdf-syntax-ns#type": "http://example.com/b"
    }, {
      "@id": "http://example.com/c",
      "http://www.w3.org/1999/02/22-rdf-syntax-ns#type": [
        "http://example.com/d",
        "http://example.com/e"
      ]
    }, {
      "@id": "http://example.com/f",
      "http://www.w3.org/1999/02/22-rdf-syntax-ns#type": "http://example.com/g"
    }
  ]
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid IRI mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    {
      "v": "http://example.com/vocab#",
      "v:term": "v:somethingElse",
      "v:termId": { "@id": "v:somethingElseId" }
    }
  ],
  "v:term": "value of v:term",
  "v:termId": "value of v:termId"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid IRI mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "./something": "http://example.com/vocab#somethingElse"
  },
  "./something": "something"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid IRI mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http:/example.org",
    "./something": {"@type": "@id", "@prefix": true}
  },
  "./something": "something"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "rev": {"@reverse": "not an IRI"}
  },
  "@id": "http://example.org/foo",
  "rev": {"@id": "http://example.org/bar"}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid IRI mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"value": "@value"},
  "http://example/prop": {"value": ["foo"]}
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid value object value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example.org/term"},
    "": {"@id": "http://example.org/empty"}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example.org/term", "@prefix": 10}
  },
  "@id": "http://example/test#example"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @prefix value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "ex:prop": {
    "@value": "value",
    "@type": ["ex:a", "ex:b"]
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid typed value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "prop": {
      "@id": "ex:prop", 
      "@type": ["ex:a", "ex:b"]
    }
  },
  "prop": "value"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid type mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@context": {
      "p": "ex:p"
    }
  },
  "@id": "ex:1",
  "p": "value"
}

)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Keyword redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example/term", "@container": ["@set"]}
  },
  "@id": "http://example/test#example",
  "term": "foo"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid container mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": {
    "term": {"@id": "http://example/term", "@container": ["@list", "@set"]}
  },
  "@id": "http://example/test#example",
  "term": "foo"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid container mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/"
  },
  "prop": "value",
  "@included": [{
    "prop": "value2"
  }]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/prop": [{"@value": "value"}],
  "@included": [{
    "http://example.org/prop": [{"@value": "value2"}]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/"
  },
  "prop": "value",
  "@included": {
    "prop": "value2"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/prop": [{"@value": "value"}],
  "@included": [{
    "http://example.org/prop": [{"@value": "value2"}]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/",
    "included1": "@included",
    "included2": "@included"
  },
  "included1": {"prop": "value1"},
  "included2": {"prop": "value2"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@included": [
    {"http://example.org/prop": [{"@value": "value1"}]},
    {"http://example.org/prop": [{"@value": "value2"}]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/"
  },
  "prop": "value",
  "@included": {
    "prop": "value2",
    "@included": {
      "prop": "value3"
    }
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/prop": [{"@value": "value"}],
  "@included": [{
    "http://example.org/prop": [{"@value": "value2"}],
    "@included": [{
      "http://example.org/prop": [{"@value": "value3"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/"
  },
  "prop": {
    "@type": "Foo",
    "@included": {
      "@type": "Bar"
    }
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/prop": [{
    "@type": ["http://example.org/Foo"],
    "@included": [{
      "@type": ["http://example.org/Bar"]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/vocab#",
    "@base": "http://example.org/base/",
    "id": "@id",
    "type": "@type",
    "data": "@nest",
    "attributes": "@nest",
    "links": "@nest",
    "relationships": "@nest",
    "included": "@included",
    "self": {"@type": "@id"},
    "related": {"@type": "@id"},
    "comments": {
      "@context": {
        "data": null
      }
    }
  },
  "data": [{
    "type": "articles",
    "id": "1",
    "attributes": {
      "title": "JSON:API paints my bikeshed!"
    },
    "links": {
      "self": "http://example.com/articles/1"
    },
    "relationships": {
      "author": {
        "links": {
          "self": "http://example.com/articles/1/relationships/author",
          "related": "http://example.com/articles/1/author"
        },
        "data": { "type": "people", "id": "9" }
      },
      "comments": {
        "links": {
          "self": "http://example.com/articles/1/relationships/comments",
          "related": "http://example.com/articles/1/comments"
        },
        "data": [
          { "type": "comments", "id": "5" },
          { "type": "comments", "id": "12" }
        ]
      }
    }
  }],
  "included": [{
    "type": "people",
    "id": "9",
    "attributes": {
      "first-name": "Dan",
      "last-name": "Gebhardt",
      "twitter": "dgeb"
    },
    "links": {
      "self": "http://example.com/people/9"
    }
  }, {
    "type": "comments",
    "id": "5",
    "attributes": {
      "body": "First!"
    },
    "relationships": {
      "author": {
        "data": { "type": "people", "id": "2" }
      }
    },
    "links": {
      "self": "http://example.com/comments/5"
    }
  }, {
    "type": "comments",
    "id": "12",
    "attributes": {
      "body": "I like XML better"
    },
    "relationships": {
      "author": {
        "data": { "type": "people", "id": "9" }
      }
    },
    "links": {
      "self": "http://example.com/comments/12"
    }
  }]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.org/base/1",
  "@type": ["http://example.org/vocab#articles"],
  "http://example.org/vocab#title": [{"@value": "JSON:API paints my bikeshed!"}],
  "http://example.org/vocab#self": [{"@id": "http://example.com/articles/1"}],
  "http://example.org/vocab#author": [{
    "@id": "http://example.org/base/9",
    "@type": ["http://example.org/vocab#people"],
    "http://example.org/vocab#self": [{"@id": "http://example.com/articles/1/relationships/author"}],
    "http://example.org/vocab#related": [{"@id": "http://example.com/articles/1/author"}]
  }],
  "http://example.org/vocab#comments": [{
    "http://example.org/vocab#self": [{"@id": "http://example.com/articles/1/relationships/comments"}],
    "http://example.org/vocab#related": [{"@id": "http://example.com/articles/1/comments"}]
  }],
  "@included": [{
    "@id": "http://example.org/base/9",
    "@type": ["http://example.org/vocab#people"],
    "http://example.org/vocab#first-name": [{"@value": "Dan"}],
    "http://example.org/vocab#last-name": [{"@value": "Gebhardt"}],
    "http://example.org/vocab#twitter": [{"@value": "dgeb"}],
    "http://example.org/vocab#self": [{"@id": "http://example.com/people/9"}]
  }, {
    "@id": "http://example.org/base/5",
    "@type": ["http://example.org/vocab#comments"],
    "http://example.org/vocab#body": [{"@value": "First!"}],
    "http://example.org/vocab#author": [{
      "@id": "http://example.org/base/2",
      "@type": ["http://example.org/vocab#people"]
    }],
    "http://example.org/vocab#self": [{"@id": "http://example.com/comments/5"}]
  }, {
    "@id": "http://example.org/base/12",
    "@type": ["http://example.org/vocab#comments"],
    "http://example.org/vocab#body": [{"@value": "I like XML better"}],
    "http://example.org/vocab#author": [{
      "@id": "http://example.org/base/9",
      "@type": ["http://example.org/vocab#people"]
    }],
    "http://example.org/vocab#self": [{"@id": "http://example.com/comments/12"}]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/"
  },
  "@included": "string"
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @included value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/"
  },
  "@included": {"@value": "value"}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @included value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/"
  },
  "@included": {"@list": ["value"]}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @included value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#bool", "@type": "@json"}
  },
  "e": true
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#bool": [{"@value": true, "@type": "@json"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#bool", "@type": "@json"}
  },
  "e": false
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#bool": [{"@value": false, "@type": "@json"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#double", "@type": "@json"}
  },
  "e": 1.23
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#double": [{"@value": 1.23, "@type": "@json"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#double", "@type": "@json"}
  },
  "e": 0.0e0
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#double": [{"@value": 0.0e0, "@type": "@json"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#integer", "@type": "@json"}
  },
  "e": 123
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#integer": [{"@value": 123, "@type": "@json"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#object", "@type": "@json"}
  },
  "e": {"foo": "bar"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#object": [{"@value": {"foo": "bar"}, "@type": "@json"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#array", "@type": "@json"}
  },
  "e": [{"foo": "bar"}]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#array": [{"@value": [{"foo": "bar"}], "@type": "@json"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#c14n", "@type": "@json"}
  },
  "e": [
    56,
    {
      "d": true,
      "10": null,
      "1": [ ]
    }
  ]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/vocab#c14n": [
      {
        "@type": "@json",
        "@value": [
          56,
          {
            "d": true,
            "10": null,
            "1": []
          }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#c14n", "@type": "@json"}
  },
  "e": {
    "peach": "This sorting order",
    "péché": "is wrong according to French",
    "pêche": "but canonicalization MUST",
    "sin":   "ignore locale"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/vocab#c14n": [
      {
        "@type": "@json",
        "@value": {
          "peach": "This sorting order",
          "péché": "is wrong according to French",
          "pêche": "but canonicalization MUST",
          "sin":   "ignore locale"
        }
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#c14n", "@type": "@json"}
  },
  "e": {
    "1": {"f": {"f": "hi","F": 5} ," ": 56.0},
    "10": { },
    "": "empty",
    "a": { },
    "111": [ {"e": "yes","E": "no" } ],
    "A": { }
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/vocab#c14n": [
      {
        "@type": "@json",
        "@value": {
          "1": {"f": {"f": "hi","F": 5} ," ": 56.0},
          "10": { },
          "": "empty",
          "a": { },
          "111": [ {"e": "yes","E": "no" } ],
          "A": { }
        }
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#c14n", "@type": "@json"}
  },
  "e": {
    "Unnormalized Unicode":"A\u030a"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/vocab#c14n": [
      {
        "@type": "@json",
        "@value": {
          "Unnormalized Unicode":"A\u030a"
        }
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#c14n", "@type": "@json"}
  },
  "e": {
    "numbers": [333333333.33333329, 1E30, 4.50, 2e-3, 0.000000000000000000000000001],
    "string": "\u20ac$\u000F\u000aA'\u0042\u0022\u005c\\\"\/",
    "literals": [null, true, false]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/vocab#c14n": [
      {
        "@type": "@json",
        "@value": {
          "numbers": [333333333.33333329, 1E30, 4.50, 2e-3, 0.000000000000000000000000001],
          "string": "\u20ac$\u000F\u000aA'\u0042\u0022\u005c\\\"\/",
          "literals": [null, true, false]
        }
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#c14n", "@type": "@json"}
  },
  "e": {
    "\u20ac": "Euro Sign",
    "\r": "Carriage Return",
    "\u000a": "Newline",
    "1": "One",
    "\u0080": "Control\u007f",
    "\ud83d\ude02": "Smiley",
    "\u00f6": "Latin Small Letter O With Diaeresis",
    "</script>": "Browser Challenge"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/vocab#c14n": [
      {
        "@type": "@json",
        "@value": {
          "\u20ac": "Euro Sign",
          "\r": "Carriage Return",
          "\u000a": "Newline",
          "1": "One",
          "\u0080": "Control\u007f",
          "\ud83d\ude02": "Smiley",
          "\u00f6": "Latin Small Letter O With Diaeresis",
          "</script>": "Browser Challenge"
        }
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#array", "@type": "@json"}
  },
  "e": [{"e": "bar"}]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#array": [{"@value": [{"e": "bar"}], "@type": "@json"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example.org/vocab#object": [{"@value": {"foo": "bar"}, "@type": "@json"}]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#object": [{"@value": {"foo": "bar"}, "@type": "@json"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"@version": 1.1, "value": "@value", "type": "@type", "json": "@json"},
  "http://example.org/vocab#object": [{"value": {"foo": "bar"}, "type": "json"}]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#object": [{"@value": {"foo": "bar"}, "@type": "@json"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#string", "@type": "@json"}
  },
  "e": "string"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#string": [{
    "@value": "string",
    "@type": "@json"
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "e": {"@id": "http://example.org/vocab#null", "@type": "@json"}
  },
  "e": null
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#null": [{
    "@value": null,
    "@type": "@json"
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "type": "@type"
  },
  "ex:foo": {
    "type": "@json",
    "@value": {
      "test": 1
    }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "ex:foo": [{
    "@type": "@json",
    "@value": {
      "test": 1
    }
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "value": "@value"
  },
  "ex:foo": {
    "@type": "@json",
    "value": {
      "test": 1
    }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "ex:foo": [{
    "@type": "@json",
    "@value": {
      "test": 1
    }
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "ex:foo": {
      "@type": "@json"
    }
  },
  "ex:foo": {
    "@context": "ex:not:a:context",
    "test": 1
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "ex:foo": [{
    "@type": "@json",
    "@value": {
      "@context": "ex:not:a:context",
      "test": 1
    }
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example.org/vocab#null": {"@value": null, "@type": "@json"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#null": [{
    "@value": null,
    "@type": "@json"
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example.org/vocab#null": {"@value": [], "@type": "@json"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/vocab#null": [{
    "@value": [],
    "@type": "@json"
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "vocab": "http://example.com/vocab/",
    "label": {
      "@id": "vocab:label",
      "@container": "@language"
    }
  },
  "@id": "http://example.com/queen",
  "label": {
    "en": null,
    "de": [ "Die Königin", null ]
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/queen",
    "http://example.com/vocab/label":
    [
      {
        "@value": "Die Königin",
        "@language": "de"
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example.com/foo": {"@list": [{"@list": ["baz"]}]}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/foo": [{"@list": [{"@list": [{"@value": "baz"}]}]}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "http://example.com/foo": {"@list": [{"@list": []}]}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/foo": [{"@list": [{"@list": []}]}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"foo": {"@id": "http://example.com/foo", "@container": "@list"}},
  "foo": [{"@list": ["baz"]}]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/foo": [{"@list": [{"@list": [{"@value": "baz"}]}]}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"foo": {"@id": "http://example.com/foo", "@container": "@list"}},
  "foo": [{"@list": []}]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/foo": [{"@list": [{"@list": []}]}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"foo": {"@id": "http://example.com/foo", "@container": "@list"}},
  "foo": [["baz"]]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/foo": [{"@list": [{"@list": [{"@value": "baz"}]}]}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"foo": {"@id": "http://example.com/foo", "@container": "@list"}},
  "foo": [[]]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/foo": [{"@list": [{"@list": []}]}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"foo": {"@id": "http://example.com/foo", "@container": "@list"}},
  "foo": [[["baz"]]]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/foo": [{"@list": [{"@list": [{"@list": [{"@value": "baz"}]}]}]}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"foo": {"@id": "http://example.com/foo", "@container": "@list"}},
  "foo": [[[]]]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/foo": [{"@list": [{"@list": [{"@list": []}]}]}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"foo": {"@id": "http://example.com/foo", "@container": "@list"}},
  "foo": [["a"], ["b"]]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/foo": [{"@list": [
    {"@list": [{"@value": "a"}]},
    {"@list": [{"@value": "b"}]}
  ]}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"foo": {"@id": "http://example.com/foo", "@container": "@list"}},
  "foo": [["a"], "b"]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/foo": [{"@list": [
    {"@list": [{"@value": "a"}]},
    {"@value": "b"}
  ]}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "idmap": {"@container": "@id"}
  },
  "idmap": {
    "http://example.org/foo": {"label": "Object with @id <foo>"},
    "_:bar": {"label": "Object with @id _:bar"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/idmap": [
    {"http://example/label": [{"@value": "Object with @id _:bar"}], "@id": "_:bar"},
    {"http://example/label": [{"@value": "Object with @id <foo>"}], "@id": "http://example.org/foo"}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "idmap": {"@container": "@id"}
  },
  "idmap": {
    "http://example.org/foo": {"@id": "http://example.org/bar", "label": "Object with @id <foo>"},
    "_:bar": {"@id": "_:foo", "label": "Object with @id _:bar"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/idmap": [
    {"@id": "_:foo", "http://example/label": [{"@value": "Object with @id _:bar"}]},
    {"@id": "http://example.org/bar", "http://example/label": [{"@value": "Object with @id <foo>"}]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "typemap": {"@container": "@type"}
  },
  "typemap": {
    "http://example.org/foo": {"label": "Object with @type <foo>"},
    "_:bar": {"label": "Object with @type _:bar"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/typemap": [
    {"http://example/label": [{"@value": "Object with @type _:bar"}], "@type": ["_:bar"]},
    {"http://example/label": [{"@value": "Object with @type <foo>"}], "@type": ["http://example.org/foo"]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "typemap": {"@container": "@type"}
  },
  "typemap": {
    "http://example.org/foo": {"@type": "http://example.org/bar", "label": "Object with @type <foo>"},
    "_:bar": {"@type": "_:foo", "label": "Object with @type _:bar"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/typemap": [
    {
      "@type": ["_:bar", "_:foo"],
      "http://example/label": [{"@value": "Object with @type _:bar"}]
    },
    {
      "@type": ["http://example.org/foo", "http://example.org/bar"],
      "http://example/label": [{"@value": "Object with @type <foo>"}]
    }
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "idmap": {"@container": "@id"}
  },
  "idmap": {
    "foo": {"label": "Object with @id <foo>"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/idmap": [
    {"http://example/label": [{"@value": "Object with @id <foo>"}], "@id": "http://example.org/foo"}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "typemap": {"@container": "@type"}
  },
  "typemap": {
    "Foo": {"label": "Object with @type <foo>"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/typemap": [
    {"http://example/label": [{"@value": "Object with @type <foo>"}], "@type": ["http://example/Foo"]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "typemap": {"@container": "@type"},
    "label": "http://example/label"
  },
  "typemap": {
    "Foo": {"label": "Object with @type <foo>"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/typemap": [
    {"http://example/label": [{"@value": "Object with @type <foo>"}], "@type": ["http://example/Foo"]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "typemap": {"@container": "@type"},
    "Type": {"@context": {"a": "http://example.org/a"}}
  },
  "typemap": {
    "Type": {"a": "Object with @type <Type>"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/typemap": [
    {"http://example.org/a": [{"@value": "Object with @type <Type>"}], "@type": ["http://example/Type"]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "vocab": "http://example.com/vocab/",
    "label": {
      "@id": "vocab:label",
      "@container": "@language"
    }
  },
  "@id": "http://example.com/queen",
  "label": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ],
    "@none": "The Queen"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/queen",
    "http://example.com/vocab/label": [
      {"@value": "The Queen"},
      {"@value": "Die Königin", "@language": "de"},
      {"@value": "Ihre Majestät", "@language": "de"},
      {"@value": "The Queen", "@language": "en"}
    ]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "vocab": "http://example.com/vocab/",
    "label": {
      "@id": "vocab:label",
      "@container": "@language"
    },
    "none": "@none"
  },
  "@id": "http://example.com/queen",
  "label": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ],
    "none": "The Queen"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/queen",
    "http://example.com/vocab/label": [
      {"@value": "Die Königin", "@language": "de"},
      {"@value": "Ihre Majestät", "@language": "de"},
      {"@value": "The Queen", "@language": "en"},
      {"@value": "The Queen"}
    ]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "idmap": {"@container": "@id"},
    "none": "@none"
  },
  "idmap": {
    "@none": {"label": "Object with no @id"},
    "none": {"label": "Another object with no @id"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/idmap": [
    {"http://example/label": [{"@value": "Object with no @id"}]},
    {"http://example/label": [{"@value": "Another object with no @id"}]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example/",
    "typemap": {"@container": "@type"},
    "none": "@none"
  },
  "typemap": {
    "@none": {"label": "Object with no @type"},
    "none": {"label": "Another object with no @type"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example/typemap": [
    {"http://example/label": [{"@value": "Object with no @type"}]},
    {"http://example/label": [{"@value": "Another object with no @type"}]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@index"]}
  },
  "input": {
    "@none": {"value": "x"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@index"]},
    "none": "@none"
  },
  "input": {
    "none": {"value": "x"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@id"]}
  },
  "input": {
    "@none": {"value": "x"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@id"]},
    "none": "@none"
  },
  "input": {
    "none": {"value": "x"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/ns/",
    "@base": "http://example.org/base/",
    "foo": { "@container": "@type" }
  },
  "foo": {"bar": "baz"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/ns/foo": [{
    "@id": "http://example.org/base/baz",
    "@type": [ "http://example.org/ns/bar" ]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/ns/",
    "@base": "http://example.org/base/",
    "foo": { "@type": "@id", "@container": "@type" }
  },
  "foo": {"bar": "baz"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/ns/foo": [{
    "@id": "http://example.org/base/baz",
    "@type": [ "http://example.org/ns/bar" ]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/ns/",
    "@base": "http://example.org/base/",
    "foo": { "@type": "@vocab", "@container": "@type" }
  },
  "foo": {"bar": "baz"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/ns/foo": [{
    "@id": "http://example.org/ns/baz",
    "@type": [ "http://example.org/ns/bar" ]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/ns/",
    "@base": "http://example.org/base/",
    "foo": { "@type": "literal", "@container": "@type" }
  },
  "foo": {"bar": "baz"}
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid type mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {"@vocab": "http://example.org/"},
  "p1": "v1",
  "@nest": {
    "p2": "v2"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/p1": [{"@value": "v1"}],
  "http://example.org/p2": [{"@value": "v2"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "nest": "@nest"
  },
  "p1": "v1",
  "nest": {
    "p2": "v2"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/p1": [{"@value": "v1"}],
  "http://example.org/p2": [{"@value": "v2"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "nest": "@nest"
  },
  "p1": "v1",
  "nest": {
    "p2": "v3"
  },
  "p2": "v2"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/p1": [{"@value": "v1"}],
  "http://example.org/p2": [
    {"@value": "v2"},
    {"@value": "v3"}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "nest1": "@nest",
    "nest2": "@nest"
  },
  "p1": "v1",
  "nest2": {
    "p2": "v4"
  },
  "p2": "v2",
  "nest1": {
    "p2": "v3"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/p1": [{"@value": "v1"}],
  "http://example.org/p2": [
    {"@value": "v2"},
    {"@value": "v3"},
    {"@value": "v4"}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/"
  },
  "p1": "v1",
  "@nest": {
    "p2": "v3",
    "@nest": {
      "p2": "v4"
    }
  },
  "p2": "v2"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/p1": [{"@value": "v1"}],
  "http://example.org/p2": [
    {"@value": "v2"},
    {"@value": "v3"},
    {"@value": "v4"}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "nest": "@nest"
  },
  "p1": "v1",
  "nest": {
    "p2": ["v4", "v5"]
  },
  "p2": ["v2", "v3"]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/p1": [{"@value": "v1"}],
  "http://example.org/p2": [
    {"@value": "v2"},
    {"@value": "v3"},
    {"@value": "v4"},
    {"@value": "v5"}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "nest": "@nest"
  },
  "p1": "v1",
  "nest": [{
    "p2": "v4"
  }, {
    "p2": "v5"
  }],
  "p2": ["v2", "v3"]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/p1": [{"@value": "v1"}],
  "http://example.org/p2": [
    {"@value": "v2"},
    {"@value": "v3"},
    {"@value": "v4"},
    {"@value": "v5"}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "nest": "@nest"
  },
  "p1": "v1",
  "nest": [
    {"@type": "T1"},
    {"@type": "T2"}
  ]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
 "@type": ["http://example.org/T1", "http://example.org/T2"],
 "http://example.org/p1": [{"@value": "v1"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    {"@vocab": "http://example/"},
    {"@version": 1.1, "a": {"@type": "@id"}}
  ],
  "a": "http://example.org/foo"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
 "http://example/a": [{"@id": "http://example.org/foo"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    {"@vocab": "http://example/"},
    {"@version": 1.1, "a": {"@type": "@id"}},
    {"b": {"@type": "@id"}}
  ],
  "a": "http://example.org/foo",
  "b": "http://example.org/bar"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
 "http://example/a": [{"@id": "http://example.org/foo"}],
 "http://example/b": [{"@id": "http://example.org/bar"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    {"@version": 1.1, "a": {"@id": "http://example/a", "@type": "@id"}},
    {"@vocab": "http://example/", "b": {"@type": "@id"}}
  ],
  "a": "http://example.org/foo",
  "b": "http://example.org/bar"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
 "http://example/a": [{"@id": "http://example.org/foo"}],
 "http://example/b": [{"@id": "http://example.org/bar"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    {"@version": 1.1, "a": {"@id": "http://example/a", "@type": "@id"}},
    {"@vocab": "http://example/"},
    {"@version": 1.1, "b": {"@type": "@id"}}
  ],
  "a": "http://example.org/foo",
  "b": "http://example.org/bar"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
 "http://example/a": [{"@id": "http://example.org/foo"}],
 "http://example/b": [{"@id": "http://example.org/bar"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.com/",
    "container": {"@container": "@index", "@index": "prop"}
  },
  "@id": "http://example.com/annotationsTest",
  "container": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ]
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.com/",
    "container": {"@index": "prop"}
  },
  "@id": "http://example.com/annotationsTest",
  "container": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ]
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.com/",
    "container": {
      "@id": "http://example.com/container",
      "@container": "@index",
      "@index": "@index"
    }
  },
  "@id": "http://example.com/annotationsTest",
  "container": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ]
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.com/",
    "container": {
      "@id": "http://example.com/container",
      "@container": "@index",
      "@index": true
    }
  },
  "@id": "http://example.com/annotationsTest",
  "container": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ]
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.com/",
    "container": {
      "@id": "http://example.com/container",
      "@container": "@index",
      "@index": "prop"
    }
  },
  "@id": "http://example.com/annotationsTest",
  "container": {
    "en": "The Queen",
    "de": [ "Die Königin", "Ihre Majestät" ]
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid value object)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@base": "http://example.com/",
    "@vocab": "http://example.com/",
    "author": {"@type": "@id", "@container": "@index", "@index": "prop"}
  },
  "@id": "article",
  "author": {
    "regular": "person/1",
    "guest": ["person/2", "person/3"]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.com/article",
  "http://example.com/author": [
    {"@id": "http://example.com/person/2", "http://example.com/prop": [{"@value": "guest"}]},
    {"@id": "http://example.com/person/3", "http://example.com/prop": [{"@value": "guest"}]},
    {"@id": "http://example.com/person/1", "http://example.com/prop": [{"@value": "regular"}]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@base": "http://example.com/",
    "@vocab": "http://example.com/",
    "author": {"@type": "@id", "@container": "@index", "@index": "prop"}
  },
  "@id": "article",
  "author": {
    "regular": {"@id": "person/1", "http://example.com/prop": "foo"},
    "guest": [
      {"@id": "person/2", "prop": "foo"},
      {"@id": "person/3", "prop": "foo"}
    ]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.com/article",
  "http://example.com/author": [
    {"@id": "http://example.com/person/2", "http://example.com/prop": [{"@value": "guest"}, {"@value": "foo"}]},
    {"@id": "http://example.com/person/3", "http://example.com/prop": [{"@value": "guest"}, {"@value": "foo"}]},
    {"@id": "http://example.com/person/1", "http://example.com/prop": [{"@value": "regular"}, {"@value": "foo"}]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@base": "http://example.com/",
    "@vocab": "http://example.com/",
    "author": {"@type": "@id", "@container": "@index", "@index": "prop"},
    "prop": {"@type": "@vocab"}
  },
  "@id": "http://example.com/article",
  "author": {
    "regular": "person/1",
    "guest": ["person/2", "person/3"]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.com/article",
  "http://example.com/author": [
    {"@id": "http://example.com/person/2", "http://example.com/prop": [{"@id": "http://example.com/guest"}]},
    {"@id": "http://example.com/person/3", "http://example.com/prop": [{"@id": "http://example.com/guest"}]},
    {"@id": "http://example.com/person/1", "http://example.com/prop": [{"@id": "http://example.com/regular"}]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@base": "http://example.com/",
    "@vocab": "http://example.com/",
    "author": {"@type": "@id", "@container": "@index", "@index": "prop"},
    "prop": {"@type": "@vocab"}
  },
  "@id": "http://example.com/article",
  "author": {
    "regular": {"@id": "person/1", "prop": "foo"},
    "guest": [
      {"@id": "person/2", "prop": "foo"},
      {"@id": "person/3", "prop": "foo"}
    ]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.com/article",
  "http://example.com/author": [
    {"@id": "http://example.com/person/2", "http://example.com/prop": [{"@id": "http://example.com/guest"}, {"@id": "http://example.com/foo"}]},
    {"@id": "http://example.com/person/3", "http://example.com/prop": [{"@id": "http://example.com/guest"}, {"@id": "http://example.com/foo"}]},
    {"@id": "http://example.com/person/1", "http://example.com/prop": [{"@id": "http://example.com/regular"}, {"@id": "http://example.com/foo"}]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@base": "http://example.com/",
    "@vocab": "http://example.com/",
    "author": {"@type": "@id", "@container": "@index", "@index": "prop"},
    "prop": {"@type": "@vocab"}
  },
  "@id": "http://example.com/article",
  "author": {
    "@none": {"@id": "person/1"},
    "guest": [
      {"@id": "person/2"},
      {"@id": "person/3"}
    ]
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "http://example.com/article",
  "http://example.com/author": [
    {"@id": "http://example.com/person/1"},
    {"@id": "http://example.com/person/2", "http://example.com/prop": [{"@id": "http://example.com/guest"}]},
    {"@id": "http://example.com/person/3", "http://example.com/prop": [{"@id": "http://example.com/guest"}]}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example.org/",
    "input": {"@container": ["@graph", "@index"], "@index": "prop"}
  },
  "input": {
    "g1": {"value": "x"}
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/input": [{
    "http://example.org/prop": [{"@value": "g1"}],
    "@graph": [{
      "http://example.org/value": [{"@value": "x"}]
    }]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.com/",
    "@version": 1.1,
    "protected": {
      "@protected": true
    }
  },
  "protected": {
    "@context": {
      "protected": "http://example.com/something-else"
    },
    "protected": "error / property http://example.com/protected"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.com/",
    "@version": 1.1,
    "protected": {
      "@protected": true
    },
    "unprotected": {
      "@protected": false
    }
  },
  "protected": true,
  "unprotected": true,
  "scope": {
    "@context": {
      "unprotected": "http://example.com/something-else"
    },
    "unprotected": "property http://example.com/something-else"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.com/protected": [{"@value": true}],
    "http://example.com/unprotected": [{"@value": true}],
    "http://example.com/scope": [{
      "http://example.com/something-else": [{"@value": "property http://example.com/something-else"}]
    }]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.com/",
    "@version": 1.1,
    "@protected": true,
    "protected1": {
      "@id": "http://example.com/protected1"
    },
    "protected2": {
      "@id": "http://example.com/protected2"
    }
  },
  "protected1": {
    "@context": {
      "protected1": "http://example.com/something-else",
      "protected2": "http://example.com/something-else"
    },
    "protected1": "error / property http://example.com/protected1",
    "protected2": "error / property http://example.com/protected2"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.com/",
    "@version": 1.1,
    "@protected": true,
    "protected": {
      "@id": "http://example.com/protected"
    },
    "unprotected": {
      "@id": "http://example.com/unprotected", "@protected": false
    }
  },
  "protected": {
    "@context": {
      "protected": "http://example.com/something-else1",
      "unprotected": "http://example.com/something-else2"
    },
    "protected": "error / property http://example.com/protected",
    "unprotected": "property http://example.com/something-else2"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.com/",
    "@version": 1.1,
    "@protected": true,
    "protected": {"@language": null}
  },
  "protected": {
    "@context": [
      null,
      {
        "@vocab": "http://something-else/"
      }
    ],
    "protected": "error / property http://example.com/protected"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid context nullification)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.com/",
    "@version": 1.1,
    "@protected": true,
    "protected": {
      "@type": "@id"
    },
    "unprotected": {
      "@protected": false,
      "@context": null
    }
  },
  "unprotected": {
    "protected": "not expanded, as protected is not a defined term"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.com/unprotected": [{}]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.com/",
    "@version": 1.1,
    "protected": {
      "@protected": false
    },
    "scope1": {
      "@protected": false,
      "@context": {
        "protected": {
          "@id": "http://example.com/something-else"
        }
      }
    },
    "scope2": {
      "@protected": true,
      "@context": {
        "protected": {
          "@protected": true
        }
      }
    }
  },
  "protected": false,
  "scope1": {
    "@context": {
      "protected": "http://example.com/another-thing"
    },
    "protected": "property http://example.com/another-thing"
  },
  "scope2": {
    "@context": {
      "protected": "http://example.com/another-thing"
    },
    "protected": "error / property http://example.com/protected"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@protected": true,
    "protected1": "http://example.org/protected1",
    "protected2": "http://example.org/protected2"
  },
  "protected2": {
    "@context": {
      "protected1": "http://example.org/something-else"
    },
    "protected1": "error / property http://example.org/protected1"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "protected": {
      "@id": "ex:protected",
      "@protected": true
    },
    "unprotected": "ex:unprotected"
  },
  "protected": "p === ex:protected",
  "unprotected": {
    "protected": "p === ex:protected"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "ex:protected": [
      {
        "@value": "p === ex:protected"
      }
    ],
    "ex:unprotected": [
      {
        "ex:protected": [
          {
            "@value": "p === ex:protected"
          }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "protected": {
      "@id": "ex:protected",
      "@protected": true
    },
    "unprotected": "ex:unprotected"
  },
  "protected": "p === ex:protected",
  "unprotected": {
    "@context": {
      "protected": "ex:protected2"
    },
    "protected": "p === ex:protected"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@protected": true,
    "protected1": "ex:protected1",
    "protected2": "ex:protected2"
  },
  "protected1": "p === ex:protected1",
  "protected2": {
    "@context": {
      "protected1": "ex:protected1:error"
    },
    "protected1": "error / p === ex:protected1"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@protected": true,
    "protected": "ex:protected",
    "unprotected": {
      "@id": "ex:unprotected1",
      "@protected": false
    }
  },
  "protected": {
    "@context": {
      "unprotected": "ex:unprotected2"
    },
    "unprotected": "p === ex:unprotected2"
  },
  "unprotected": "p === ex:unprotected1"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "ex:protected": [
      {
        "ex:unprotected2": [
          {
            "@value": "p === ex:unprotected2"
          }
        ]
      }
    ],
    "ex:unprotected1": [
      {
        "@value": "p === ex:unprotected1"
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@protected": true,
    "protected1": "ex:protected1",
    "protected2": {
      "@id": "ex:protected2",
      "@context": null
    }
  },
  "protected1": "p === ex:protected1",
  "protected2": {
    "@context": {
      "protected1": "ex:protected3"
    },
    "protected1": "p === ex:protected3"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "ex:protected1": [
      {
        "@value": "p === ex:protected1"
      }
    ],
    "ex:protected2": [
      {
        "ex:protected3": [
          {
            "@value": "p === ex:protected3"
          }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@protected": true,
    "protected1": "ex:protected1",
    "protected2": {
      "@id": "ex:protected2",
      "@context": [
        null
      ]
    }
  },
  "protected1": "p === ex:protected1",
  "protected2": {
    "@context": {
      "protected1": "ex:protected3"
    },
    "protected1": "p === ex:protected3"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "ex:protected1": [
      {
        "@value": "p === ex:protected1"
      }
    ],
    "ex:protected2": [
      {
        "ex:protected3": [
          {
            "@value": "p === ex:protected3"
          }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@protected": true,
    "protected1": "ex:protected1",
    "protected2": {
      "@id": "ex:protected2",
      "@context": [
        null,
        {
          "protected1": "ex:protected3",
          "unprotected": "ex:unprotected2"
        }
      ]
    },
    "unprotected": {
      "@protected": false,
      "@id": "ex:unprotected1"
    }
  },
  "protected1": "p === ex:protected1",
  "protected2": {
    "@context": {
      "protected1": "ex:protected3"
    },
    "protected1": "p === ex:protected3",
    "unprotected": "p === ex:unprotected2"
  },
  "unprotected": "p === ex:unprotected1"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "ex:protected1": [
      {
        "@value": "p === ex:protected1"
      }
    ],
    "ex:protected2": [
      {
        "ex:protected3": [
          {
            "@value": "p === ex:protected3"
          }
        ],
        "ex:unprotected2": [
          {
            "@value": "p === ex:unprotected2"
          }
        ]
      }
    ],
    "ex:unprotected1": [
      {
        "@value": "p === ex:unprotected1"
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@protected": true,
    "protected1": "ex:protected1",
    "protected2": "ex:protected2",
    "Protected": {
      "@id": "ex:Protected",
      "@context": [
        null
      ]
    }
  },
  "protected1": "p === protected1",
  "protected2": {
    "@context": {
      "unprotected": "ex:unprotected"
    },
    "@type": "Protected",
    "unprotected": "error / omitted"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid context nullification)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@protected": true,
    "protected": "ex:protected1",
    "Protected": {
      "@id": "ex:Protected",
      "@context": [
        null,
        {
          "protected": "ex:protected2"
        }
      ]
    }
  },
  "@type": "Protected",
  "protected": "error / p === ex:protected1"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid context nullification)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@protected": true,
    "protected1": "ex:protected1",
    "protected2": {
      "@id": "ex:protected2",
      "@context": [
        null,
        {
          "protected1": "ex:protected3"
        }
      ]
    },
    "unprotected": {
      "@protected": false,
      "@id": "ex:unprotected1"
    }
  },
  "protected1": "p === ex:protected1",
  "protected2": {
    "@context": {
      "protected1": "ex:protected3"
    },
    "protected1": "p === ex:protected3",
    "unprotected": "p === ex:unprotected2"
  },
  "unprotected": "p === ex:unprotected1"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "ex:protected1": [
      {
        "@value": "p === ex:protected1"
      }
    ],
    "ex:protected2": [
      {
        "ex:protected3": [
          {
            "@value": "p === ex:protected3"
          }
        ]
      }
    ],
    "ex:unprotected1": [
      {
        "@value": "p === ex:unprotected1"
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@protected": true,
    "protected1": "ex:protected1",
    "Protected": {
      "@id": "ex:Protected",
      "@context": [
        null,
        {
          "protected1": "ex:protected2",
          "unprotected": "ex:unprotected2"
        }
      ]
    },
    "unprotected": {
      "@protected": false,
      "@id": "ex:unprotected1"
    }
  },
  "@type": "Protected",
  "protected1": "error / p === ex:protected1",
  "unprotected": "p === ex:unprotected2"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid context nullification)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@protected": true,
    "protected": "ex:protected",
    "Protected": {
      "@id": "ex:Protected",
      "@context": [
        null
      ]
    },
    "unprotected": {
      "@protected": false,
      "@id": "ex:unprotected"
    }
  },
  "@type": "Protected",
  "protected": "error / p === ex:protected",
  "unprotected": "missing"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid context nullification)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@version": 1.1,
    "@protected": true,
    "@vocab": "http://example.com/",
    "Parent": {"@context": {"@protected": true, "foo": {"@type": "@id"}}}
  }, {
    "@version": 1.1,
    "@protected": true,
    "Child": {"@context": {"@protected": true, "foo": {"@type": "@id"}}}
  }],
  "@type": "Parent",
  "foo": {
    "@type": "Child",
    "foo": "http://example.com/test"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@type": [
      "http://example.com/Parent"
    ],
    "http://example.com/foo": [
      {
        "@type": [
          "http://example.com/Child"
        ],
        "http://example.com/foo": [
          {
            "@id": "http://example.com/test"
          }
        ]
      }
    ]
  }
]

)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@version": 1.1,
    "@protected": true,
    "id": "@id",
    "type": "@type"
  }, {
    "@version": 1.1,
    "@protected": true,
    "id": "@id",
    "type": "@type"
  }],
  "id": "http://example/id",
  "type": "http://example/type"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example/id",
    "@type": [
      "http://example/type"
    ]
  }
]

)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@version": 1.1,
    "@protected": true,
    "foo": "http://example/foo#"
  }, {
    "@version": 1.1,
    "@protected": true,
    "foo": "http://example/foo#"
  }],
  "foo:bar": "foobar"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example/foo#bar": [{
      "@value": "foobar"
    }]
  }
]

)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@version": 1.1,
    "@protected": true,
    "id": "@id",
    "type": "@type",
    "foo": {
      "@id": "http://example/foo",
      "@container": ["@graph", "@set"]
    },
    "bar": {
      "@id": "http://example/bar",
      "@type": "@id",
      "@context": {
        "@version": 1.1,
        "@protected": true,
        "bar-1": {
          "@id": "http://example/bar-1",
          "@context": {
            "@version": 1.1,
            "@protected": true,
            "bar-2": "http://example/bar-2",
            "Foo": {
              "@id": "http://example/Foo",
              "@context": {
                "@version": 1.1,
                "@protected": true,
                "bar-2": "http://example/bar-2"
              }
            }
          }
        }
      }
    },
    "Bar": {
      "@id": "http://example/Bar",
      "@context": {
        "@version": 1.1,
        "@protected": true,
        "bar-1": {
          "@id": "http://example/bar-1",
          "@context": {
            "@version": 1.1,
            "@protected": true,
            "bar-2": "http://example/bar-2",
            "Foo": {
              "@id": "http://example/Foo",
              "@context": {
                "@version": 1.1,
                "@protected": true,
                "bar-2": "http://example/bar-2"
              }
            }
          }
        }
      }
    },
    "Foo": {
      "@id": "http://example/Foo",
      "@context": {
        "@version": 1.1,
        "@protected": true,
        "bar-2": "http://example/bar-2"
      }
    }
  }, {
    "@version": 1.1,
    "@protected": true,
    "id": "@id",
    "type": "@type",
    "foo": {
      "@id": "http://example/foo",
      "@container": ["@graph", "@set"]
    },
    "bar": {
      "@id": "http://example/bar",
      "@type": "@id",
      "@context": {
        "@version": 1.1,
        "@protected": true,
        "bar-1": {
          "@id": "http://example/bar-1",
          "@context": {
            "@version": 1.1,
            "@protected": true,
            "bar-2": "http://example/bar-2",
            "Foo": {
              "@id": "http://example/Foo",
              "@context": {
                "@version": 1.1,
                "@protected": true,
                "bar-2": "http://example/bar-2"
              }
            }
          }
        }
      }
    },
    "Bar": {
      "@id": "http://example/Bar",
      "@context": {
        "@version": 1.1,
        "@protected": true,
        "bar-1": {
          "@id": "http://example/bar-1",
          "@context": {
            "@version": 1.1,
            "@protected": true,
            "bar-2": "http://example/bar-2",
            "Foo": {
              "@id": "http://example/Foo",
              "@context": {
                "@version": 1.1,
                "@protected": true,
                "bar-2": "http://example/bar-2"
              }
            }
          }
        }
      }
    },
    "Foo": {
      "@id": "http://example/Foo",
      "@context": {
        "@version": 1.1,
        "@protected": true,
        "bar-2": "http://example/bar-2"
      }
    }
  }],
  "type": "Bar",
  "foo": [{
    "bar": "http://example/"
  }],
  "bar-1": {
    "bar-2": {
      "type": "Foo",
      "bar-2": "bar-2"
    }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@type": [
      "http://example/Bar"
    ],
    "http://example/foo": [{
      "@graph": [{
        "http://example/bar": [{
          "@id": "http://example/"
        }]
      }]
    }],
    "http://example/bar-1": [{
      "http://example/bar-2": [{
        "@type": ["http://example/Foo"],
        "http://example/bar-2": [{
          "@value": "bar-2"
        }]
      }]
    }]
  }
]

)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@version": 1.1,
    "@protected": true,
    "id": "@id",
    "type": "@type",
    "foo": {
      "@id": "http://example/foo",
      "@container": ["@graph", "@set"]
    },
    "bar": {
      "@id": "http://example/bar",
      "@type": "@id",
      "@context": {
        "@version": 1.1,
        "@protected": true,
        "bar-1": {
          "@id": "http://example/bar-1",
          "@context": {
            "@version": 1.1,
            "@protected": true,
            "bar-2": "http://example/bar-2",
            "Foo": {
              "@id": "http://example/Foo",
              "@context": {
                "@version": 1.1,
                "@protected": true,
                "bar-2": "http://example/bar-2"
              }
            }
          }
        }
      }
    },
    "Bar": {
      "@id": "http://example/Bar",
      "@context": {
        "@version": 1.1,
        "@protected": true,
        "bar-1": {
          "@id": "http://example/bar-1",
          "@context": {
            "@version": 1.1,
            "@protected": true,
            "bar-2": "http://example/bar-2",
            "Foo": {
              "@id": "http://example/Foo",
              "@context": {
                "@version": 1.1,
                "@protected": true,
                "bar-2": "http://example/bar-2"
              }
            }
          }
        }
      }
    },
    "Foo": {
      "@id": "http://example/Foo",
      "@context": {
        "@version": 1.1,
        "@protected": true,
        "bar-2": "http://example/bar-2"
      }
    }
  }, {
    "@version": 1.1,
    "@protected": true,
    "id": "@id",
    "type": "@type",
    "foo": {
      "@id": "http://example/foo",
      "@container": ["@graph", "@set"]
    },
    "bar": {
      "@id": "http://example/bar",
      "@type": "@id",
      "@context": {
        "@version": 1.1,
        "@protected": true,
        "bar-1": {
          "@id": "http://example/bar-1",
          "@context": {
            "@version": 1.1,
            "@protected": true,
            "bar-2": "http://example/bar-2",
            "Foo": {
              "@id": "http://example/Foo",
              "@context": {
                "@version": 1.1,
                "@protected": true,
                "bar-2": "http://example/bar-2"
              }
            }
          }
        }
      }
    },
    "Bar": {
      "@id": "http://example/Bar",
      "@context": {
        "@version": 1.1,
        "@protected": true,
        "bar-1": {
          "@id": "http://example/bar-1",
          "@context": {
            "@version": 1.1,
            "@protected": true,
            "bar-2": "http://example/bar-2",
            "Foo": {
              "@id": "http://example/Foo",
              "@context": {
                "@version": 1.1,
                "@protected": true,
                "bar-2": "http://example/bar-2"
              }
            }
          }
        }
      }
    },
    "Foo": {
      "@id": "http://example/Foo"
    }
  }],
  "type": "Bar",
  "foo": [{
    "bar": "http://example/"
  }],
  "bar-1": {
    "bar-2": {
      "type": "Foo",
      "bar-2": "bar-2"
    }
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@version": 1.1,
    "@protected": true,
    "id": "@id",
    "type": "@type"
  }, {
    "@version": 1.1,
    "id": "@id",
    "type": "@type"
  }],
  "id": "http://example/id",
  "type": "http://example/type"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example/id",
    "@type": [
      "http://example/type"
    ]
  }
]

)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@version": 1.1,
    "@protected": true,
    "term": null
  }, {
    "@version": 1.1,
    "term": {"@id": "http://example.com/term"}
  }],
  "term": "undefined"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "tag": {"@id": "http://example.org/ns/tag/", "@prefix": false}
  },
  "tag:champin.net,2019:prop": "This is not treated as a Compact IRI",
  "tag": "tricky"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "http://example.org/ns/tag/": [
      {
        "@value": "tricky"
      }
    ],
    "tag:champin.net,2019:prop": [
      {
        "@value": "This is not treated as a Compact IRI"
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "id": {"@id": "@id", "@protected": true},
    "type": {"@id" : "@type", "@container": "@set", "@protected" : true},
    "@type": {"@container": "@set", "@protected": true}
  },
  "id": "http://example.com/1",
  "type": "http://example.org/ns/Foo",
  "@type": "http://example.org/ns/Bar"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "http://example.com/1",
    "@type": [
      "http://example.org/ns/Bar",
      "http://example.org/ns/Foo"
    ]
  }
])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@version": 1.1,
    "id": {"@id": "@id", "@protected": true},
    "type": {"@id" : "@type", "@container": "@set", "@protected" : true},
    "@type": {"@container": "@set", "@protected": true}
  }, {
    "@version": 1.1,
    "id": "http://example.com/id"
  }],
  "id": "http://example.com/1",
  "type": ["http://example.org/ns/Foo"]
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@version": 1.1,
    "id": {"@id": "@id", "@protected": true},
    "type": {"@id" : "@type", "@container": "@set", "@protected" : true},
    "@type": {"@container": "@set", "@protected": true}
  }, {
    "@version": 1.1,
    "@type": {"@protected": true}
  }],
  "id": "http://example.com/1",
  "type": ["http://example.org/ns/Foo"]
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "foo": {
      "@id": "@type",
      "@prefix": true
    }
  },
  "foo:bar": "http://example.org/baz"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid term definition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@ignoreMe": "http://example.org/should-ignore"
  },
  "@type": "http://example.com/IgnoreTest",
  "@ignoreMe": "should not be here"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["http://example.com/IgnoreTest"]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "@ignoreMe": "http://example.org/should-ignore"
  },
  "@type": "http://example.com/IgnoreTest",
  "@ignoreMe": "should not be here"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["http://example.com/IgnoreTest"]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "ignoreMe": "@ignoreMe"
  },
  "@type": "http://example.com/IgnoreTest",
  "ignoreMe": "should not be here"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["http://example.com/IgnoreTest"]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "ignoreMe": "@ignoreMe"
  },
  "@type": "http://example.com/IgnoreTest",
  "ignoreMe": "vocabulary relative"
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["http://example.com/IgnoreTest"],
  "http://example.org/ignoreMe": [{"@value": "vocabulary relative"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "ignoreMe": {"@reverse": "@ignoreMe"}
  },
  "@type": "http://example.com/IgnoreTest",
  "ignoreMe": {"http://example.org/text": "should not be here"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["http://example.com/IgnoreTest"]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.org/",
    "ignoreMe": {"@reverse": "@ignoreMe"}
  },
  "@type": "http://example.com/IgnoreTest",
  "ignoreMe": {"text": "not reversed"}
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@type": ["http://example.com/IgnoreTest"],
  "http://example.org/ignoreMe": [{
    "http://example.org/text": [{"@value": "not reversed"}]
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://vocab.org/",
    "@protected": true,
    "bar": "http://ignored.org/bar",
    "foo": {
      "@context": {
        "bar": "http://example.org/other"
      }
    }
  },
  "@id": "ex:outer",
  "foo": {
    "@id": "ex:inner",
    "bar": "baz"
  }
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "ex:outer",
  "http://vocab.org/foo": [{
    "@id": "ex:inner",
    "http://example.org/other": [{"@value": "baz"}]
  }]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@protected": true,
    "foo": {
      "@id": "ex:foo"
    }
  },
  "@id": "ex:outer",
  "foo": {
    "@context": {
      "@protected": true,
      "foo": "ex:foo"
    },
    "@id": "ex:inner"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "@id": "ex:outer",
  "ex:foo": [{
    "@id": "ex:inner"
  }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [
    {
      "@protected": true,
      "protected": "ex:protected"
    },
    {
      "protected": "ex:protected"
    },
    {
      "protected": "ex:unprotected"
    }
  ],
  "protected": "fail / should retain protection during redefinition"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@protected": true,
    "protected": {
      "@id": "ex:protected1",
      "@type": "@id"
    },
    "unprotected": {
      "@id": "ex:unprotected",
      "@type": "@id",
      "@container": "@graph",
      "@context": null
    }
  },
  "@id": "ex:outer",
  "unprotected": {
    "@context": {
      "protected": "ex:protected2"
    },
    "@id": "ex:inner",
    "protected": "p === ex:protected2"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@id": "ex:outer",
    "ex:unprotected": [
      {
        "@graph": [
          {
            "@id": "ex:inner",
            "ex:protected2": [
              {
                "@value": "p === ex:protected2"
              }
            ]
          }
        ]
      }
    ]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@import": "so01-in.jsonld"
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid context entry)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@import": {}
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid @import value)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@import": "so03-in.jsonld"
  }
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid context entry)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example/",
    "Foo": {
      "@context": {
        "@import": "so05-context.jsonld",
        "@propagate": true
      }
    }
  },
  "@type": "Foo",
  "bar": {"baz": "buzz"}
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([
  {
    "@type": ["http://example/Foo"],
    "http://example/bar": [{
      "http://example.org/baz": [{"@id": "http://example/buzz"}]
    }]
  }
]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@vocab": "http://example/",
    "bar": {
      "@context": {
        "@import": "so06-context.jsonld",
        "@propagate": false
      }
    }
  },
  "bar": {"baz": {"baz": "buzz"}}
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
 "http://example/bar": [{
   "http://example.com/baz": [{
     "http://example/baz": [{"@value": "buzz"}]
   }]
 }]
}]
)jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@vocab": "http://example.com/",
    "@version": 1.1,
    "@protected": true,
    "@import": "so07-context.jsonld"
  },
  "protected1": {
    "@context": {
      "protected1": "http://example.com/something-else",
      "protected2": "http://example.com/something-else"
    },
    "protected1": "error / property http://example.com/protected1",
    "protected2": "error / property http://example.com/protected2"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@import": "so08-context.jsonld",
    "term": "http://example.org/redefined"
  },
  "term": "value"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/redefined": [{"@value": "value"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@import": "so09-context.jsonld",
    "@vocab": "http://example.org/redefined/"
  },
  "term": "value"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/redefined/term": [{"@value": "value"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": [{
    "@version": 1.1,
    "@protected": true,
    "@import": "so10-context.jsonld"
  }, {
    "term": "http://example.org/unprotected"
  }],
  "term": "value"
}
  )jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Protected term redefinition)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@protected": true,
    "@import": "so08-context.jsonld",
    "term": "http://example.org/redefined"
  },
  "term": "value"
}
)jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.org/redefined": [{"@value": "value"}]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@import": "so12-in.jsonld"
  }
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid context entry)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "@import": "so13-context.jsonld"
  },
  "term": "value"
}
)jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid remote context)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_1},
    KnownCase{.input = R"jld({
  "@context": {
    "xsd": "http://www.w3.org/2001/XMLSchema#",
    "notype": {"@id": "http://example.com/notype", "@type": "@none"}
  },
  "notype": [
    "string",
    true,
    false,
    1,
    10.0,
    {"@value": "plain"},
    {"@value": true, "@type": "xsd:boolean"},
    {"@value": "english", "@language": "en"},
    {"@value": "2018-02-17", "@type": "xsd:date"},
    {"@id": "http://example.com/iri"}
  ]
})jld",
              .expand_context = nullptr,
              .expected = nullptr,
              .error_code = R"jld(Invalid type mapping)jld",
              .pointer = R"jld()jld",
              .version = sourcemeta::core::JSONLDVersion::V1_0},
    KnownCase{.input = R"jld({
  "@context": {
    "@version": 1.1,
    "xsd": "http://www.w3.org/2001/XMLSchema#",
    "notype": {"@id": "http://example.com/notype", "@type": "@none"}
  },
  "notype": [
    "string",
    true,
    false,
    1,
    10.0,
    {"@value": "plain"},
    {"@value": true, "@type": "xsd:boolean"},
    {"@value": "english", "@language": "en"},
    {"@value": "2018-02-17", "@type": "xsd:date"},
    {"@id": "http://example.com/iri"}
  ]
})jld",
              .expand_context = nullptr,
              .expected = R"jld([{
  "http://example.com/notype": [
    {"@value": "string"},
    {"@value": true},
    {"@value": false},
    {"@value": 1},
    {"@value": 10.0},
    {"@value": "plain"},
    {"@value": true, "@type": "http://www.w3.org/2001/XMLSchema#boolean"},
    {"@value": "english", "@language": "en"},
    {"@value": "2018-02-17", "@type": "http://www.w3.org/2001/XMLSchema#date"},
    {"@id": "http://example.com/iri"}
  ]
}])jld",
              .error_code = nullptr,
              .pointer = nullptr,
              .version = sourcemeta::core::JSONLDVersion::V1_1},
}};

