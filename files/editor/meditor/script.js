function form_submit(name,row,col){
	if(name==null) var name="part_body";
	var w=$("#editor_"+name);
	var x=ace.edit("editor_"+name).getValue();
    	$("#"+name).val(x);
	var shadow_box=$("#"+name+"_editor_shadow");
	var z=shadow_box.html();
	var c=parseInt($("#"+name+"_toolbar").height());
	shadow_box.show();
	shadow_box.css("top",window.scrollY);
	shadow_box.css("left","0");
	shadow_box.offset(w.offset());
	shadow_box.css("top",(parseInt(shadow_box.css("top"))-c)+"px");
	shadow_box.width(w.width()+2);
	shadow_box.height(w.height()+c+2);
	shadow_box.html("<div style=\"position: relative; top: "+(w.offset().top+w.height()-184)+"px;\"><div style=\"width: 150px; margin-bottom: 10px; padding: 10px; background-color: #1076DC; color: #FFFFFF;\">Сохранение...</div></div>");
	shadow_box.show();
	
	var ajax_vars={
		action: "save_part",
		body: x
	};
	if(typeof(row)!='undefined' && row!=0){		
		ajax_vars.row=row;
		ajax_vars.col=col;
	} else {
		ajax_vars.part=save_part;
	}
	$.post(jsq2+"/ajax", ajax_vars,
	function(data,status){
		if(data=="1"){				
			shadow_box.hide();
			shadow_box.html(z);
		} else {
			shadow_box.hide();
			shadow_box.html(z);			
			alert("Не удалось сохранить");
		}
	});
}

function heightUpdateFunction(name){
	var editor=editors[name];
	var oldHeight=$("#editor_"+name).height();
	var newHeight=editor.getSession().getLength() * editor.renderer.lineHeight+18/* + editor.renderer.scrollBar.getWidth()*/;
	if(newHeight<160) newHeight=160;
	$("#editor_"+name).height(newHeight+"px");
	$("#editor_"+name+' .ace_scroller').height(newHeight+"px");
	/*var w1=parseInt($("#editor_"+name+" .ace_scroller").css("width"));
	var w2=parseInt($("#editor_"+name+" .ace_content").css("width"));
	if(w1<w2){
		$("#editor_"+name).width(w2+"px");
		$("#editor_"+name+" .ace_scroller").width(w2+"px");
	}*/
	$("#"+name+"_scroll_box").width((parseInt($("#editor_"+name+" .ace_scroller").css("width"))+18)+"px");
	$("#"+name+"_scroll_content").width((parseInt($("#editor_"+name+" .ace_content").width())/*+50*/)+"px");
	if(oldHeight!=newHeight) editor.resize();
	//$("#editor_"+name+' .ace_scroller').css('right',"0px");
}

function onScrollUpdate(name){
	var editor=editors[name];
	
	var a=parseInt($(document).scrollTop());
	var b=parseInt($(window).height());
	var c=parseInt($("#"+name+"_toolbar").height());
	var d=parseInt($("#editor_"+name).height());
	var x=a+b;
	var y=parseInt($("#editor_"+name).position().top);
	var z=y+d;
	
	if(editor.renderer.$horizScroll){			
		$("#"+name+"_scroll_box").width((parseInt($("#editor_"+name+" .ace_scroller").css("width"))/*+35*/+18)+"px");
		$("#"+name+"_scroll_content").width((parseInt($("#editor_"+name+" .ace_content").width())/*+50*/)+"px");
	
		if(x<z && x>y+100) $("#"+name+"_scroll_box").show();
		else $("#"+name+"_scroll_box").hide();
	} else $("#"+name+"_scroll_box").hide();
	
	if(y<a+c){
		if(a-y+c<d-100){
			$("#"+name+"_toolbar").css("height","35px");
			$("#"+name+"_toolbar").css("top",(a-y+c)+"px");
			$("#editor_"+name+" .ace_search").css("top",(a-y+c+5)+"px");
		}
	} else {
		$("#"+name+"_toolbar").css("top","0px");
		$("#"+name+"_toolbar").css("height","40px");
		$("#editor_"+name+" .ace_search").css("top","0px");
	}
}

function onScrollReverseUpdate(name){
	if(noScroll2) return false;
	var editor=editors[name];
	if(editor.renderer.$horizScroll){
		noScroll=true;
		var old=$("#"+name+"_scroll_box").css("display");
		$("#"+name+"_scroll_box").css("display","block");
		$("#"+name+"_scroll_box").get(0).scrollLeft=$("#editor_"+name+" .ace_scrollbar-h").get(0).scrollLeft;
		$("#"+name+"_scroll_box").css("display",old);
	}
}

function fakeScroll(obj,name){
	noScroll2=true; 
	if(!noScroll){
		var editor=editors[name];
		$("#editor_"+name+" .ace_content").css('marginLeft',-obj.scrollLeft+'px');
		$("#editor_"+name+" .ace_scrollbar-h").get(0).scrollLeft=obj.scrollLeft/*-50*/;
		if(obj.scrollLeft>0) $("#editor_"+name+" .ace_scroller").addClass('ace_scroll-left');
		else $("#editor_"+name+" .ace_scroller").removeClass('ace_scroll-left');
	}
	noScroll=false;
	noScroll2=false;
}

//HotSave
var handlers=[];
function addHandler(object, event, handler, useCapture){
	if(event in handlers) return false;
	handlers[handlers.length]=event;
	if (object.addEventListener) {
		object.addEventListener(event, handler, useCapture ? useCapture : false);
	} else if (object.attachEvent) {
		object.attachEvent("on" + event, handler);
	}
}

var ua = navigator.userAgent.toLowerCase();
var isIE = (ua.indexOf("msie") != -1 && ua.indexOf("opera") == -1);
var isGecko = (ua.indexOf("gecko") != -1); 
var isChrome = (ua.indexOf("chrome") != -1); 

 if (isIE || isChrome) addHandler(document, "keydown", hot_key_down);
 else addHandler(document, "keypress", hot_key_down);
 
addHandler(document, "keyup", hot_key_up);
 
 var hotkeys=[];

 function hot_key_down(evt) {
	evt = evt || window.event;
	var key = evt.keyCode || evt.which;
	tkey = String.fromCharCode(key).toLowerCase() == "s";
	if (evt.ctrlKey && tkey) {
		if(evt.preventDefault) evt.preventDefault();
		evt.returnValue = false;
		
		//form_submit();
		if(editors!=undefined){
			for(var key in editors){
				var e=editors[key];
				if(e.can_save==1 && e.isFocused()) form_submit(e.editor_name,e.edit_row,e.edit_col);
			}
		}
		return false;
	}
	if('down' in hotkeys)
	for(var hk in hotkeys['down']){
		var t=hotkeys['down'][hk];
		if(t(evt,key)) return false;
	}
}

 function hot_key_up(evt) {
	evt = evt || window.event;
	var key = evt.keyCode || evt.which;
	if('up' in hotkeys)
	for(var hk in hotkeys['up']){
		var t=hotkeys['up'][hk];
		if(t(evt,key)) return false;
	}
}

function show_op_editor(name, parent){
	managers[managers.length]=new manager(0,function(value){
		paste_editor(editors[name],value,value.length,0);
	});
}

function show_op_editor_text(id,type,result,level,title,additional){
	if(result<0){
		type=-result;
		result=-result;
	}
	managers[managers.length]=new manager(type,function(value){
		insertAtCursor(document.getElementById(id),value);
	},result,level,title,additional);
}

function show_op_editor_ckeditor(id){
	if(typeof(x_cmd)=='undefined') refresh_data();
	managers[managers.length]=new manager(CMD_BASE,function(value){	
		for(var key in CKEDITOR.instances){
			if(CKEDITOR.instances[key].name==id){
				CKEDITOR.instances[key].insertHtml('['+value+']');
				break;
			}
		}
	},0,0,'Вставить значение','');
}

function isNumeric(n) {
	return !isNaN(parseFloat(n)) && isFinite(n);
}

function paste_editor(editor,value,offsetCursor,offsetSelection){
	var selectionRange = editor.getSelectionRange();
	var startLine = selectionRange.start.row;
	var endLine = selectionRange.end.row;
	var x = editor.session.getTextRange(selectionRange);
	editor.session.remove(selectionRange);
	//var x=editor.getSession().getSelectedText();
	var position=editor.selection.getCursor();			
	if(offsetCursor) position.column=position.column+offsetCursor;
	if(x!="" && offsetSelection) value=value.substr(0,offsetSelection)+x+value.substr(offsetSelection);
	editor.getSession().insert(editor.selection.getCursor(),value);
	editor.selection.clearSelection();
	editor.selection.moveCursorToPosition(position);
	editor.focus();
}

function load_json(json_url){
	var res='';
	$.ajax({
		url: json_url,
		type: "GET",
		success: function(content){ res=content; },
		dataType: "json",
		async: false
	});
	return res;
}

function refresh_data(){
	x_cmd=load_json(jsq2+"/ajax?action=get_im_json&name=cmd");
	x_component=load_json(jsq2+"/ajax?action=get_im_json&name=component");
	x_widget=load_json(jsq2+"/ajax?action=get_im_json&name=widget");
	if(selectModuleSname){
		x_module=load_json(jsq2+"/ajax?action=get_im_json&name="+selectModuleSname);
	} else x_module=0;
}

(function($){
  $.fn.extend({ 
    onShow: function(callback, unbind){
      return this.each(function(){
        var _this = this;
        var bindopt = (unbind==undefined)?true:unbind; 
        if($.isFunction(callback)){
          if($(_this).is(':hidden')){
            var checkVis = function(){
              if($(_this).is(':visible')){
                callback.call(_this);
                if(bindopt){
                  $('body').unbind('click keyup keydown', checkVis);
                }
              }                         
            }
            $('body').bind('click keyup keydown', checkVis);
          }
          else{
            callback.call(_this);
          }
        }
      });
    }
  });
})(jQuery);