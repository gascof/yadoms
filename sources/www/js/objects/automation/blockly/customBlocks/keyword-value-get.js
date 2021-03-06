
/**
 * Define a block which allow to read a keyword value
 * @type {{init: Function}}
 */
Blockly.Blocks["keyword-value-get"] = {
    deviceDropDownName: "Device",
    keywordDropDownName: "Keyword",
    init: function () {

        //set custom block parameters
        this.setInputsInline(true);
        this.setOutput(true);
        this.setTooltip($.t("blockly.blocks.keyword-value-get.tooltip", { defaultValue: "" }));
		this.setHelpUrl(Blockly.Yadoms.GenerateHelpUrl(this.type));

        var thisBlock = this;
        Blockly.Yadoms.ConfigureBlockForYadomsKeywordSelection(thisBlock, false, ["numeric", "string", "bool", "enum", "datetime"], undefined, function (keyword, keywordType) {

            var typeToSet = keywordType;
            if (typeToSet == null)
                typeToSet = "null"; //any type allowed

            //make it compatible with older blockly version
			try {
				$.isFunction(thisBlock.changeOutput) ? thisBlock.changeOutput(typeToSet) : thisBlock.outputConnection.setCheck(typeToSet);
			}catch(e) { }//just catch it.

            Blockly.Yadoms.UpdateBlockColour_(thisBlock, keywordType);
        }, this.deviceDropDownName, this.keywordDropDownName);
    }
};


