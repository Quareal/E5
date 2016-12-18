/*
Copyright (c) 2003-2010, CKSource - Frederico Knabben. All rights reserved.
For licensing, see LICENSE.html or http://ckeditor.com/license
*/

CKEDITOR.editorConfig = function( config )
{
	// Define changes to default configuration here. For example:
	// config.language = 'fr';
	// config.uiColor = '#AADC6E';
	config.resize_minwidth = "90%";
	//config.removePlugins = 'resize'
	config.entities = false;
	config.htmlEncodeOutput = false;
	config.disableNativeSpellChecker = false;
	config.skin = 'office2003';
	config.toolbar_Full = [ 
		['Source','Preview','Save','Undo','Redo','Maximize','ShowBlocks','-','RemoveFormat','SelectAll','Cut','Copy','Paste','PasteText','PasteFromWord'],
		['Subscript','Superscript','Blockquote','NumberedList','BulletedList','-','Image','Table','Link','Unlink'],
		,'/',
		['Bold','Italic','-','JustifyLeft','JustifyCenter','JustifyRight','JustifyBlock'],['Format','Styles','Font','FontSize','TextColor','BGColor']
	];
	config.toolbar_Zero = [ ];
	config.toolbar_Mini = [ ['Save','Maximize','Bold','Italic'] ];
	config.toolbar = 'Full';
	config.filebrowserBrowseUrl='/files/js/elfinder/elfinder.html';
};
