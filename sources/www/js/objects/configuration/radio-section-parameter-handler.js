/**
 * Created by Nicolas on 01/03/14.
 */

/**
 * Create a radio section parameter handler
 * @param i18nContext
 * @param paramName
 * @param content
 * @param currentValue
 * @constructor
 */
function RadioSectionParameterHandler(i18nContext, paramName, content, currentValue, parentRadioButtonSectionName, parentRadioSectionActive) {
   assert(i18nContext !== undefined, "i18nContext must contain path of i18n");
   assert(paramName !== undefined, "paramName must be defined");
   assert(content !== undefined, "content must be defined");
   assert(Object.keys(content.content).length >= 2, "You must have at least two sub sections into a radioSection");

   this.configurationHandlers = [];
   this.configurationValues = currentValue;
   this.name = content.name;
   this.paramName = paramName;
   this.description = isNullOrUndefined(content.description)?"":content.description;
   this.i18nContext = i18nContext;
   this.content = content;
   this.containerUuid = createUUID();
   this.uuid = createUUID();
   var self = this;

   //we look for parent radio button
   if (parentRadioButtonSectionName !== undefined) {
      this.parentRadioButtonSectionName = parentRadioButtonSectionName;
      this.parentRadioSectionActive = false;
      if (parentRadioSectionActive !== undefined)
         this.parentRadioSectionActive = parseBool(parentRadioSectionActive);
   }
   
   if (isNullOrUndefined(self.configurationValues))
      self.configurationValues = {};

   if (isNullOrUndefined(self.configurationValues.activeSection)) {
      //for the moment no section have been defined so we select the first one
      self.configurationValues.activeSection = Object.keys(content.content)[0];
   }


   //for each key in package
   $.each(content.content, function (key, value) {
      var v = undefined;
      if ((self.configurationValues.content != null) && (self.configurationValues.content != undefined))
         v = self.configurationValues.content[key];

      var newI18nContext = i18nContext + self.paramName + ".content.";

      //all items in content must be section values
      assert(ConfigurationHelper.isContainer(value), "Content section of the configuration " + self.name + " must contain only section items");

      var radioActive = false;
      if (self.configurationValues.activeSection == key)
         radioActive = true;

      if (value.show !== undefined && value.show.result === "false")
         return;
      
	    var handler = ConfigurationHelper.createParameterHandler(newI18nContext, key, value, v, self.uuid, radioActive);
      if (!isNullOrUndefined(handler))
         self.configurationHandlers.push(handler);
   });
}

/**
 * Get the DOM Object to insert
 * @returns {string}
 */
RadioSectionParameterHandler.prototype.getDOMObject = function () {

   var input = "<div class=\"control-group configuration-radio-section well\" id=\"" + this.uuid + "\">" +
                  "<div class=\"configuration-header\" >";
	
   if (this.parentRadioButtonSectionName) {
      input +=       "<div class=\"radio\">" +
                        "<label>" +
                           "<input type=\"radio\" id=\"" + this.selectorUuid + "\" name=\"" + this.parentRadioButtonSectionName + "\" ";
      if (this.parentRadioSectionActive)
         input +=                "checked ";
      input +=             ">";
   }
	
   input +=           "<span data-i18n=\"" + this.i18nContext + this.paramName + ".name\" >" +
                        this.name +
                     "</span>";
   //if it's included in a parent radioSection 
   if (this.parentRadioButtonSectionName) {
      input +=          "</label>" +
                     "</div>";
   }
   
   input +=          "</div>" +
                  "<div class=\"configuration-description\" data-i18n=\"" + this.i18nContext + this.paramName + ".description\" >" +
                     this.description +
                  "</div>" +
                  "<div class=\"toggle-btn-grp\" id=\"" + this.containerUuid + "\" >";

   //for each param in the section we create a radio button and a div that contain the item
   $.each(this.configurationHandlers, function (key, value) {
      input +=             value.getDOMObject();
   });

   input +=       "</div>" +
               "</div>";

   return input;
};

RadioSectionParameterHandler.prototype.locateInDOM = function () {
   return $("div#" + this.uuid);
};

/**
 * Get the param name
 * @returns {string}
 */
RadioSectionParameterHandler.prototype.getParamName = function() {
  return this.paramName;
};

/**
 * Apply script after DOM object has been added to the page
 */
RadioSectionParameterHandler.prototype.applyScript = function () {
   var self = this;

   if (self.parentRadioButtonSectionName) {
      $("#" + self.selectorUuid).change(function () {
         if ($("input#" + self.selectorUuid + ":checked").val() == "on") {
            //we hide all sections-content in the radioSection\n" +
		var $parentSection = $("div#" + self.parentRadioButtonSectionName);
            var radioSections = $parentSection.find(" > div.toggle-btn-grp > div.configuration-section > div.section-content");
            radioSections.addClass("hidden");
            $parentSection.removeClass("has-warning has-error");
            $parentSection.find("input,select,textarea").removeClass("enable-validation");
		//Disable all existing sub-buttons
		$parentSection.find("button").attr("disabled", true);

            //We save all items that are "required" with a special class name : required-for-validation
            var $requiredFields = radioSections.find("[required]");
            //we remove attr required and save it using class "required-for-validation"
            $requiredFields.addClass("required-for-validation");
            $requiredFields.removeAttr("required");

            //we show current
            var $activeContainer = $("div#" + self.containerUuid);
            $activeContainer.removeClass("hidden");
            $activeContainer.find("input,select,textarea").addClass("enable-validation");
            //Enable all existing sub-buttons
		$activeContainer.find("button").removeAttr("disabled");
            //we restore required items
            $activeContainer.find(".required-for-validation").attr("required", "required");
         }
      });
   }
   
   //we apply script in each children
   $.each(this.configurationHandlers, function (key, value) {
      if ($.isFunction(value.applyScript))
         value.applyScript();
   });

   //can't factorize must happen after intialize all handlers
   $.each(this.configurationHandlers, function (key, value) {
      //if this child is the active one we fire the event that we have changed to it the radio button
      if (value.parentRadioSectionActive)
         $("#" + value.selectorUuid).change();
   });

 
};

/**
 * Get the current configuration in the form
 * @returns {object}
 */
RadioSectionParameterHandler.prototype.getCurrentConfiguration = function () {
   //we update configurationValues with content of DOM
   var self = this;
   self.configurationValues = {};
   self.configurationValues.content = {};
   $.each(self.configurationHandlers, function (key, value) {
      var currentConfiguration = value.getCurrentConfiguration();
      self.configurationValues.content[value.getParamName()] = currentConfiguration;
      if (currentConfiguration.radio) {
         //it's the active section
         self.configurationValues.activeSection = value.getParamName();
      }
   });

   //we get the parentRadioButtonSectionName value if used (only for nested radio section into another one)
   if (this.parentRadioButtonSectionName) {
      self.configurationValues.radio = ($("input#" + this.selectorUuid + ":checked").val() == 'on');
   }

   return self.configurationValues;
};