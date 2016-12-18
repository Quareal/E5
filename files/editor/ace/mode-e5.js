define('ace/mode/e5', function(require, exports, module) {

var oop = require("ace/lib/oop");
var TextMode = require("ace/mode/text").Mode;
var Tokenizer = require("ace/tokenizer").Tokenizer;
var E5HighlightRules = require("ace/mode/e5_highlight_rules").E5HighlightRules;

var Mode = function() {
    var highlighter = new E5HighlightRules();
    this.$tokenizer = new Tokenizer(highlighter.getRules());
};
oop.inherits(Mode, TextMode);

(function() {
    
}).call(Mode.prototype);

exports.Mode = Mode;
});

define('ace/mode/e5_highlight_rules', function(require, exports, module) {

var oop = require("ace/lib/oop");
var TextHighlightRules = require("ace/mode/text_highlight_rules").TextHighlightRules;

var E5HighlightRules = function() {

    this.$rules = {
    	start:[{
            token : "meta.tag", // opening tag
            regex : "<\\/?",
            next : "tag"
        }, {
            token : "text",
            regex : "\\s+"
        }, {
            token : "constant.character.entity",
            regex : "(?:&#[0-9]+;)|(?:&#x[0-9a-fA-F]+;)|(?:&[a-zA-Z0-9_:\\.-]+;)"
        }]
    };

}

oop.inherits(E5HighlightRules, TextHighlightRules);

exports.E5HighlightRules = E5HighlightRules;
});