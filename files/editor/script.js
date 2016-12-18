var dq='"';
var IE = document.all?true:false;
var box1=0, box2=0, box3=0, box4=0; 
var lmi=0; var otype=0;

function MM_swapImgRestore() { //v3.0
  var i,x,a=document.MM_sr; for(i=0;a&&i<a.length&&(x=a[i])&&x.oSrc;i++) x.src=x.oSrc;
}

function MM_preloadImages() { //v3.0
  var d=document; if(d.images){ if(!d.MM_p) d.MM_p=new Array();
    var i,j=d.MM_p.length,a=MM_preloadImages.arguments; for(i=0; i<a.length; i++)
    if (a[i].indexOf("#")!=0){ d.MM_p[j]=new Image; d.MM_p[j++].src=a[i];}}
}

function MM_findObj(n, d) { //v4.01
  var p,i,x;  if(!d) d=document; if((p=n.indexOf("?"))>0&&parent.frames.length) {
    d=parent.frames[n.substring(p+1)].document; n=n.substring(0,p);}
  if(!(x=d[n])&&d.all) x=d.all[n]; for (i=0;!x&&i<d.forms.length;i++) x=d.forms[i][n];
  for(i=0;!x&&d.layers&&i<d.layers.length;i++) x=MM_findObj(n,d.layers[i].document);
  if(!x && d.getElementById) x=d.getElementById(n); return x;
}

function MM_swapImage() { //v3.0
  var i,j=0,x,a=MM_swapImage.arguments; document.MM_sr=new Array; for(i=0;i<(a.length-2);i+=3)
   if ((x=MM_findObj(a[i]))!=null){document.MM_sr[j++]=x; if(!x.oSrc) x.oSrc=x.src; x.src=a[i+2];}
}

function setstyle(id, attribute, value){
	var e = document.getElementById(id);
	eval('e.style.'+attribute+'="'+value+'"');
}

function str_replace(search, replace, subject){
	return subject.split(search).join(replace);
}
 
function replace_content(div1,div2,a1,a2,b1,b2,c1,c2,d1,d2){
	var val=document.getElementById(div1).innerHTML;
	if(a1!='') val=str_replace(a1,a2,val);
	if(b1!='') val=str_replace(b1,b2,val);
	if(c1!='') val=str_replace(c1,c2,val);
	if(d1!='') val=str_replace(d1,d2,val);
	try{document.getElementById(div2).innerHTML=val;}catch(e){}
}

function flipimg(id,i1,i2){
	var obj=document.getElementById(id);
	if(obj.src.indexOf(i2)!=-1) obj.src=i1; else obj.src=i2;
}

function showhide(id){
	var e = document.getElementById(id);
	if(e===null) return '';
	if((e.style.display && e.style.display == 'none') || (!e.style.display && e.className && e.className.indexOf('hidden',0)>=0)) 
	{
		if(e.className.indexOf('hidden',0)>=0) e.style.display = 'block';
		else e.style.display='';
	}
	else
	{
		e.style.display = 'none';
	}
	return 0;
}

function show(id){
	var e = document.getElementById(id);
	if(e!=null) e.style.display = '';
	return 0;
}

function hide(id){
	var e = document.getElementById(id);
	if(e!=null) e.style.display = 'none';
	return 0;
}

function clear(id){
	var e = document.getElementById(id);
	e.innerHTML = '';
	return 0;
}

var burl='';

function showhide2(id){
	var e = document.getElementById(id);
	if(e.style.display && e.style.display == 'none') 
	{
		setstyle('wrapper', 'margin', '0 0 0 233px');
		setstyle('top', 'background', burl);
		e.style.display = 'block';
	}
	else
	{
		burl=document.getElementById('top').style.background;
		setstyle('top', 'background', 'none');
		e.style.display = 'none';
		setstyle('wrapper', 'margin', '0');
	}
	return 0;
}

if(document.getElementsByClassName == undefined) { 
	document.getElementsByClassName = function(cl) { 
		var retnode = []; 
		var myclass = new RegExp('\\b'+cl+'\\b'); 
		var elem = this.getElementsByTagName('*'); 
		for (var i = 0; i < elem.length; i++) { 
			var classes = elem[i].className; 
			if (myclass.test(classes)) { 
				retnode.push(elem[i]); 
			} 
		} 
		return retnode; 
	}
};

function selr(id){
	var obj=document.getElementById('r'+id);
	if(obj.className=='') obj.className='act';
	else if(obj.className=='na') obj.className='na act';
	else if(obj.className=='na act') obj.className='na';
	else obj.className='';
}

function selr2(id,act){
	var obj=document.getElementById('r'+id);
	if(obj==null) return;
	if(act==1){
		if(obj.className=='') obj.className='act';
		if(obj.className=='na') obj.className='na act';
	} else {
		if(obj.className=='act') obj.className='';
		if(obj.className=='na act') obj.className='na';
	}
}

function addLoadEvent(func) {
        var oldonload = window.onload;
        if (typeof window.onload != 'function') {
                //window.onload = func;
	window.onload = function() {
	      eval(func);
                        //func();
                 }
        }
        else {
                window.onload = function() {
                        oldonload();
	      eval(func);
                        //func();
                }
        }
}

function JSQ(id,type){
	img=document.getElementById("p-"+id);
	hid=document.getElementById(id);
	tmp=hid.value;
	if(tmp==-1){ hid.value=0; img.src=jsq+'/files/editor/deny.gif'; }
	if(tmp==0){ hid.value=1; img.src=jsq+'/files/editor/allow.gif'; }
	if(tmp==1 && type==0){ hid.value=-1; img.src=jsq+'/files/editor/up.gif'; }
	if(tmp==1 && type==2){ hid.value=2; img.src=jsq+'/files/editor/user3.gif'; }
	if(tmp==2 && type==2){ hid.value=3; img.src=jsq+'/files/editor/group.gif'; }
	if(tmp==3 && type==2){ hid.value=-1; img.src=jsq+'/files/editor/up.gif'; }
	if(tmp==5 && type==5){ hid.value=-1; img.src=jsq+'/files/editor/up.gif'; }
	if(tmp==1 && type==5){ hid.value=5; img.src=jsq+'/files/editor/allow_all.gif'; }
}

function JSQ2(id,type){
	img=document.getElementById("p-"+id);
	hid=document.getElementById(id);
	tmp=hid.value;
	if(tmp==-1){ hid.value=1; img.src=jsq+'/files/editor/deny.gif'; }
	if(tmp==1 && type==2){ hid.value=2; img.src=jsq+'/files/editor/user3.gif'; }
	if(tmp==2 && type==2){ hid.value=-1; img.src=jsq+'/files/editor/up.gif'; }
}

function JSQ3(id,type){
	img=document.getElementById("p-"+id);
	hid=document.getElementById(id);
	tmp=hid.value;
	if(tmp==-1){ hid.value=1; img.src=jsq+'/files/editor/deny.gif'; }
	if(tmp==1 && type==2){ hid.value=2; img.src=jsq+'/files/editor/user3.gif'; }
	if(tmp==2 && type==2){ hid.value=3; img.src=jsq+'/files/editor/group.gif'; }
	if(tmp==3 && type==2){ hid.value=-1; img.src=jsq+'/files/editor/up.gif'; }
}

function explode(delimiter,string){
	var emptyArray = { 0: '' };
	if ( arguments.length != 2
		|| typeof arguments[0] == 'undefined'
		|| typeof arguments[1] == 'undefined' )
	{
		return null;
	}
	if ( delimiter === ''
		|| delimiter === false
		|| delimiter === null )
	{
		return false;
	}
	if ( typeof delimiter == 'function'
		|| typeof delimiter == 'object'
		|| typeof string == 'function'
		|| typeof string == 'object' )
	{
		return emptyArray;
	}
	if ( delimiter === true ) {
		delimiter = '1';
	}
	return string.toString().split ( delimiter.toString() );
}

function java_sub(tid,id){
	var res=list1+tid+list2;
	for (var i = 0; i < list3a.length; i++) {
	    if(list3a[i]!=id){
	    	var bool=true;
	    	for (var i2 = 0; i2 < list3c[i].length; i2++) if(list3c[i][i2]==id) bool=false;
	    	if(bool==true) res=res+'<option '+list3b[i];
	    	else res=res+'<option disabled '+list3b[i];
	    } else {
	    	res=res+'<option disabled '+list3b[i];
	    }
	}
	res=res+list4;
	if(list5!=''){
		res=res+list5+tid+list6;
	}
	res=res+list7+id+list8+tid+list9;
	if(list5=='') res=res+list10;
	else {
		res=res+'+\'&id5=\'+document.getElementById(\'tir'+tid+'\').value'+list10;
	}
	var obj=document.getElementById('pan'+tid);
	obj.innerHTML=res;
	showhide('pan'+tid);
}

function getCaretPosition (ctrl) {
	var CaretPos = 0;
	// IE Support
	if (document.selection) {
 
		ctrl.focus ();
		var Sel = document.selection.createRange ();
 
		Sel.moveStart ('character', -ctrl.value.length);
 
		CaretPos = Sel.text.length;
	}
	// Firefox support
	else if (ctrl.selectionStart || ctrl.selectionStart == '0')
		CaretPos = ctrl.selectionStart;
	return (CaretPos);
}
 
function setCaretPosition(ctrl, pos){
	if(ctrl.setSelectionRange){
		ctrl.focus();
		ctrl.setSelectionRange(pos,pos);
	}
	else if (ctrl.createTextRange) {
		var range = ctrl.createTextRange();
		range.collapse(true);
		range.moveEnd('character', pos);
		range.moveStart('character', pos);
		range.select();
	}
}

function insertAtCursor(myField, myValue) {   
	cp=getCaretPosition(myField);
	if (document.selection) {   
		myField.focus();   
		sel = document.selection.createRange();   
		sel.text = myValue;   
	}   
	else if (myField.selectionStart || myField.selectionStart == '0') {   
		var startPos = myField.selectionStart;   
		var endPos = myField.selectionEnd;   
		myField.value = myField.value.substring(0, startPos) + myValue + myField.value.substring(endPos, myField.value.length);   
		myField.selectionStart=startPos+myValue.length;
	} else {   
		myField.value += myValue;   
	}  
	setCaretPosition(myField,cp+myValue.length);
}

function translate(text,lng,resf){
	google.language.detect(text, function(result) {
		var language = 'unknown';
		for (l in google.language.Languages) {
			if (google.language.Languages[l] == result.language) {
				language = l;
				break;
			}
		}
		google.language.translate(text, result.language, lng, function(result) {
			resf(result.translation);
		});
	});
}

function translate2(elem1,elem2){
	if(elem1.value=='') return true;
	if(elem2.value!='') return true;
	elem2.value=translate_latinica(elem1.value);
}

function translate_latinica(src){
	var res=src;
	var rus='ЙЦУКЕЁНГШЩЗХФЫВАПРОЛДЖЭЯЧСМИТБЮ1234567890';
	var rus2='йцукеёнгшщзхфывапролджэячсмитбю';
	var eng='icukeengsszhfivaproldgeacsmitbu1234567890';
	var except='qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM';
	var repl=' ()-';
	var rusb=Array();
	var rusc=Array();
	for(i=0;i<rus.length;i++){
		rusb[rus.substr(i,1).charCodeAt(0)]=eng.substr(i,1);
		rusc[rus2.substr(i,1).charCodeAt(0)]=eng.substr(i,1);
	}
	var res2='';
	for(i=0;i<res.length;i++){
		if(except.indexOf(res.substr(i,1))!=-1) res2+=res.substr(i,1);
		else if(repl.indexOf(res.substr(i,1))!=-1) res2+='_';
		else if(rusb[res.substr(i,1).charCodeAt(0)]!=null) res2+=rusb[res.substr(i,1).charCodeAt(0)];
		else if(rusc[res.substr(i,1).charCodeAt(0)]!=null) res2+=rusc[res.substr(i,1).charCodeAt(0)];
	}
	return res2.toLowerCase();	
}

function icon_click(o){
	var obj=document.getElementById('part_pic');
	obj.value=o;
	obj=document.getElementById('part_pic2');
	obj.src='/files/editor/icons/'+o;
	showhide('icondiv');
}

function unbind_elfinder(){
	$(document).unbind($.browser.opera ? 'keypress' : 'keydown');
}

var fm;
function show_elfinder(callback,folder){
    if(!folder) folder='';
    fm = new elFinder(document.getElementById('finder'), {
	url : jsq+'/files/js/elfinder/connectors/php/connector.php?folder='+folder,
	lang : 'ru',
	dialog : { width : 900, modal : true, title : 'Файлы' },
 	        closeOnEditorCallback : true,
	        editorCallback : function(url) {
	               	if(callback!=null) callback(url);
	        	//$(document).unbind($.browser.opera ? 'keypress' : 'keydown');
	        	fm.lockShortcuts(true);
	        }
    });
    $('.ui-icon-closethick').bind('click',function(){
    	//fm.remove();
    	fm.lockShortcuts(true);
    	/*$(document).unbind($.browser.opera ? 'keypress' : 'keydown');
    	$(document).unbind('keydown');
   	$(document).unbind('keyup');
    	$(document).unbind('keypress');*/
    });
    $(document).bind('keydown',function(e){
    	var e=e || window.event;
    	var key = e.keyCode || e.which;
    	if(key==27){
    		fm.lockShortcuts(true);
    	}
    });
    //setTimeout('unbind_elfinder()',3000);
}

function up_from_serv(url,id){
	$('#'+id).hide();
	$('#'+id+'_browse').hide();
	$('#'+id+'_serv').attr('value',url);
	$('#'+id+'_info').attr('innerHTML',url);
	$('#'+id+'_block').show();
}

function up_from_pc(id){
	$('#'+id+'_block').hide();
	$('#'+id).show();
	$('#'+id+'_browse').show();
	$('#'+id+'_serv').attr('value','');
}

function getXmlHttpA(){
	var xmlhttp;
	if(xmlhttp!=null) return;
	try {
		xmlhttp = new ActiveXObject("Msxml2.XMLHTTP");
	} catch (e) {
		try {
			xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
		} catch (E) {
			xmlhttp = false;
		}
	}
	if (!xmlhttp && typeof XMLHttpRequest!='undefined') {
		xmlhttp = new XMLHttpRequest();
	}
	return xmlhttp;
}

function loadurlA(url,id,name,val){
       var xmlhttp = getXmlHttpA();
       var obj=document.getElementById(id);
	xmlhttp.open("POST", url, true);
           xmlhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
	xmlhttp.onreadystatechange=function(){
	  if (xmlhttp.readyState != 4) return;
	  clearTimeout(timeout);
	  if (xmlhttp.status == 200) {
                 if(name=='' && val==''){
                     if(obj.innerHTML!=='')
                       obj.innerHTML=xmlhttp.responseText;
                     else
                       obj.value=xmlhttp.responseText;
                 }
	  } else {
	      handleError(xmlhttp.statusText);
	  }
	}
           if(name!=''){
                var obj2=document.getElementById(name);
                xmlhttp.send("val="+encodeURIComponent(obj2.value));
           } else if(val!=''){
                xmlhttp.send("val="+encodeURIComponent(val));
           } else xmlhttp.send("a=5&b=4");
	var timeout = setTimeout( function(){ xmlhttp.abort(); handleError("Time over") }, 10000);	
	function handleError(message) {
	   obj.innerHTML='Ошибка запроса. Обновите страницу для продолжения';
	}
}

function loadurlB(cmd,resf,val1,val2,val3,val4,val5,val6){
	var xmlhttp = getXmlHttpA();
	xmlhttp.open("POST", jsq2+'/ajax', true);
	xmlhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
	xmlhttp.onreadystatechange=function(){
		if (xmlhttp.readyState != 4) return;
		clearTimeout(timeout);
		if (xmlhttp.status == 200) {
			resf(xmlhttp.responseText);
		}
	}
	xmlhttp.send("action="+cmd+"&x="+encodeURIComponent(val1)+"&y="+encodeURIComponent(val2)+"&z="+encodeURIComponent(val3)+"&a="+encodeURIComponent(val4)+"&b="+encodeURIComponent(val5)+"&c="+encodeURIComponent(val6));
	var timeout = setTimeout( function(){ xmlhttp.abort() }, 10000);
}

function loadurlC(cmd,res_id,val1,val2,val3){
	var xmlhttp = getXmlHttpA();
	xmlhttp.open("POST", jsq2+'/ajax', true);
	xmlhttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
	xmlhttp.onreadystatechange=function(){
		if (xmlhttp.readyState != 4) return;
		clearTimeout(timeout);
		if (xmlhttp.status == 200) {
			//document.getElementById(res_id).innerHTML=xmlhttp.responseText;
			$('#'+res_id).append(xmlhttp.responseText);
		}
	}
	xmlhttp.send("action="+cmd+"&x="+encodeURIComponent(val1)+"&y="+encodeURIComponent(val2)+"&z="+encodeURIComponent(val3));
	var timeout = setTimeout( function(){ xmlhttp.abort() }, 10000);	
}

function shell_event(element,eventName){
 /*var event;
  if (document.createEvent) {
    event = document.createEvent("HTMLEvents");
    event.initEvent("dataavailable", true, true);
  } else {
    event = document.createEventObject();
    event.eventType = "dataavailable";
  }

  event.eventName = eventName;
  //event.memo = memo || { };

  if (document.createEvent) {
    element.dispatchEvent(event);
  } else {
    element.fireEvent("on" + event.eventType, event);
  }*/
  var event = new Event(eventName, {bubbles : true, cancelable : true});
  element.dispatchEvent(event);
}

function change_own_un(id6){
	var i=1;
	var edit="";
	var obj=document.getElementById("cso"+i);
	while(obj!=null){
		edit=obj.name.substr(3);
		obj.value=id6;
		edit=document.getElementById(edit);
		shell_event(edit,"blur");
		i=i+1;
		obj=document.getElementById("cso"+i);
	}
}

function own_sel1(t){	
	var id=t.options[t.selectedIndex].value;
	var obj=document.getElementById("ro_owner2");
	change_own_un(id);
	if(obj!=null){
		obj.value=document.getElementById("ro_owner2d").value;
	}
}

function own_sel2(t){
	var id=t.options[t.selectedIndex].value;
	change_own_un(id);
	var obj=document.getElementById("ro_owner");
	if(obj!=null){
		obj.value=0;
	}
}

var tab="\t";
function insertTab(evt) {	
    var t = evt.target;
    var ss = t.selectionStart;
    var se = t.selectionEnd;
    var line_height=16;
    //ss=se;
    var scroll=t.scrollTop;
    var scroll2=t.scrollLeft;
    if (evt.keyCode == 9 || evt.keyCode == 13) {
    	if(ss!=se){
    		insertAtCursor2(t,'');
    		se=ss;
    	}
    }
    if (evt.keyCode == 9) {
        evt.preventDefault();               
        if (ss != se && t.value.slice(ss,se).indexOf("\n") != -1) {
        	// эти секции больше не нужны
            var pre = t.value.slice(0,ss);
            var sel = t.value.slice(ss,se).replace(/\n/g,"\n"+tab);
            var post = t.value.slice(se,t.value.length);
            t.value = pre.concat(tab).concat(sel).concat(post);
            t.selectionStart = ss + tab.length;
            t.selectionEnd = /*se*/ss + tab.length;
            t.scrollTop=scroll;
            t.scrollLeft=scroll2;
        }
        else {
            t.value = t.value.slice(0,ss).concat(tab).concat(t.value.slice(ss,t.value.length));
            if (ss == se) {
            	t.disable=true;
                t.selectionStart = ss + tab.length;
                t.selectionEnd = ss + tab.length;
                t.scrollTop=scroll;
                                             
                var pre = t.value.slice(0,ss+1);
                pre=explode(String.fromCharCode(10),pre);
                pre=pre[pre.length-1];
                var data=pre;
                
                var span = document.createElement('div');
                span.style.fontSize=$(t).css('font-size');
                span.style.tabSize=$(t).css('tab-size');
                span.style.fontFamily=$(t).css('font-family');
                span.style.float='left';
                span.style.overflow = 'auto';
		span.style.wrap = 'hard';
		span.style.whiteSpace = 'pre';
		span.style.position = 'absolute';
		span.style.zIndex = -10;
                span.innerHTML = htmlspecialchars2(data);
                document.body.appendChild(span);
                var x=span.offsetWidth;
                document.body.removeChild(span);
                if($.browser.msie){var zx=x*1.8}//всё равно криво :(
                else var zx=x;
                //document.title=(t.offsetWidth+scroll2)+' > '+(zx+50);

                if(t.offsetWidth+scroll2>zx+50) t.scrollLeft=scroll2;
                else {
                	t.scrollLeft=x-15;
                }
            }
            else {
	        // эти секции больше не нужны
                t.selectionStart = ss + tab.length;
                t.selectionEnd = /*se*/ss + tab.length;
                t.scrollTop=scroll;
                t.scrollLeft=scroll2;
            }
        }
    }
    else if(evt.keyCode==13){
    	var zi=0;
	var ti=0;
	var ot=(t.value.charAt(ss)==tab);
    	for(var i=ss-1;i>=0;i--)if(i>=0){
    		if((t.value.charAt(i)==String.fromCharCode(13) || t.value.charAt(i)==String.fromCharCode(10) || i==0) && !ot){
    			if(i==0) zi=0; else zi=i+1;
    			while (t.value.charAt(zi)==tab && zi<=ss){ ti++; zi++;}
    			break;
    		} else if(t.value.charAt(i)!=tab) ot=false;
    	}
    	if(ti>0){
    		evt.preventDefault();
    		var ta=String.fromCharCode(13);
    		for(var i=0;i<ti;i++) ta=ta+tab;

	        if (ss != se && t.value.slice(ss,se).indexOf("\n") != -1) {
      	           // эти секции больше не нужны
	            var pre = t.value.slice(0,ss);
	            var sel = t.value.slice(ss,se).replace(/\n/g,"\n"+ta);
	            var post = t.value.slice(se,t.value.length);
	            t.value = pre.concat(ta).concat(sel).concat(post);
	            t.selectionStart = ss + ta.length;
	            t.selectionEnd = /*se*/ss + ta.length;
	            t.scrollTop=scroll;
	            t.scrollLeft=scroll2;
	        }
	        else {
	            t.value = t.value.slice(0,ss).concat(ta).concat(t.value.slice(ss,t.value.length));
	            if (ss == se) {
	            	if($.browser.opera){
		                t.selectionStart = ss + ta.length+1;
		                t.selectionEnd = ss + ta.length+1;
		        } else {
		                t.selectionStart = ss + ta.length;
		                t.selectionEnd = ss + ta.length;
		        }

		        t.scrollLeft=scroll2;
	                var pre=t.value.slice(0,ss);
	                var cline=substr_count(pre,"\n")+2;
	                var sline=Math.floor(scroll/line_height);
	                //if (scroll!=0 && scroll % line_height!=0) cline++;
    	                //alert((scroll % line_height));
                	//if (t.clientHeight % line_height>0) cline++;
	                if(scroll>0){
	                	sline++;
	                }
	                //if(pre[1]=="\n") cline--;
	                var mline=cline-sline+4;
	                //alert(t.clientHeight+' '+(mline*line_height));
	                //alert(mline*line_height+' '+t.clientHeight);
	                if(mline*line_height<=t.clientHeight) t.scrollTop=scroll;
	                else {
	                	//t.scrollTop=scroll+line_height;
	                	t.scrollTop=(cline+2)*line_height-t.clientHeight/*+10*/-t.clientHeight % line_height;
	                }		        
		        
	            }
	            else {
 	               // эти секции больше не нужны
	                t.selectionStart = ss + ta.length;
	                t.selectionEnd = /*se*/ss + ta.length;
	                t.scrollTop=scroll;
	                t.scrollLeft=scroll2;
	            }
	        }
    		
    	}
    }
}

function ffv(id){
	var val=document.getElementById(id).value;
	return val;
}

var last_pager=Array();
last_pager["o"]=1;
last_pager["t"]=1;
last_pager["s"]=1;
function pager_over(id,type){
	var obj=document.getElementById(type+"pager"+id);
	obj.style.backgroundColor="#1076DC";
	obj.style.color="#FFFFFF";
}

function pager_out(id,type,force){
	if(id!=last_pager[type] || force){
		var obj=document.getElementById(type+"pager"+id);
		obj.style.backgroundColor="#E6EFF6";
		obj.style.color="#000000";
	}
}

function pager_click(id,type){
	var obj=document.getElementById(type+"page"+last_pager[type]);
	obj.style.display="none";
	var obj=document.getElementById(type+"page"+id);
	obj.style.display="";
	pager_out(last_pager[type],type,1);
	last_pager[type]=id;
	pager_over(id,type);
}

function visit_month_select(o){
	var xmonth=o.options[o.selectedIndex].value;
	var month=xmonth % 12;
	var year=(xmonth-month)/12+2010+1;
	var start_day=new Date(year,month-1,1); 
	var start_day2=new Date(2010,0,1);	
	start_day=start_day.getTime();
	start_day2=start_day2.getTime();
	var xday=Math.floor((start_day-start_day2)/1000/60/60/24);
	var day_count=33 - new Date(year, month-1, 33).getDate();
	var sels=document.getElementsByName('select_day')[0];
	for(var i=sels.options.length-1;i>=0;i--) sels.remove(i);
	for(var i=1;i<=day_count;i++){
		var w=new Date(year,month,i).getDay();
		switch(w){
			case 0:w2='вс';break;
			case 1:w2='пн';break;
			case 2:w2='вт';break;
			case 3:w2='ср';break;
			case 4:w2='чт';break;
			case 5:w2='пт';break;
			case 6:w2='сб';break;
		}
		sels.options[i-1]=new Option(i+' ('+w2+')',xday+i-1);
	}
}

function load_users(callback){
	$.post(jsq2+"/ajax", {
		action: "load_users"
	},
	function(data,status){
		if(data!=""){
			dataA='<select id="se%id%" name="new_user" style="margin: 0px; margin-top: 5px;">'+data+'</select>';
			dataB=data;
			$('#usel-container').html(dataA);
			$('#new_own2').html(dataB);
			callback();
		} else {
		}
	});
}

function click_user(tid,nu,row_user,row_id,sv,dq){
	if($('#usel-container').html()==''){
		load_users(function(){click_user(tid,nu,row_user,row_id,sv,dq);});
	} else {
		showhide('udiv'+tid);
		replace_content('usel','u2div'+tid,'%url%',nu,dq+row_user+dq,dq+row_user+dq+' selected','%id%',row_id,'%sv%',sv);
	}
}

function change_select(id,ajax,select){
	var sv=select.options[select.selectedIndex];
	var val=sv.value;
	if(val=='new'){
		clear('addrow'+id);
		loadurlC('add_row_form', 'addrow'+id, ajax);
		show('addrow'+id);
	} else hide('addrow'+id)
	var hrefer=$('#hrefer'+id);
	var href=$(sv).attr('meta');
	/*alert(val);alert(href);*/
	if(val!=0 && val!='new' && href!=''){
		href=jsq2+'/'+href;
		hrefer.attr('href',href);
		hrefer.show();
	} else {
		hrefer.hide();
	}
}

function getAbsolutePosition(el) {
	var r = { x: el.offsetLeft, y: el.offsetTop };
	if (el.offsetParent) {
		var tmp = getAbsolutePosition(el.offsetParent);
		r.x += tmp.x;
		r.y += tmp.y;
	}
	return r;
}

function showwnd(obj,type){
	var wnd=document.getElementById('menu');
	var wnds=document.getElementById('menu_shadow');
	if(wnd.style.display=='' && otype==type){
		wnd.style.display='none';
		wnds.style.display='none';
	} else {
		if(document.getElementById('cmenu'+type)){
			wnd.style.display='';
			wnds.zIndex=5;
			wnds.style.display='';
			if (IE) document.getElementById("menu_shadow").style.filter = "progid:DXImageTransform.Microsoft.Alpha(Opacity=0)";
			else document.getElementById("menu_shadow").style.opacity = 0.0;
			wnd.style.left=(getAbsolutePosition(obj).x-(wnd.offsetWidth/2-obj.offsetWidth/2))+'px';
			wnd.style.top=getAbsolutePosition(obj).y+14+'px';
			if(document.getElementById('cmenu'+type)) document.getElementById('mcontent').innerHTML=document.getElementById('cmenu'+type).innerHTML;
			else document.getElementById('mcontent').innerHTML='';
		}
	}
	otype=type;
}

function hidewnd(){
	var wnd=document.getElementById('menu');
	var wnds=document.getElementById('menu_shadow');
	wnds.style.display='none';
        wnd.style.display='none';
}

function itemover(obj){
	if(lmi!=0){lmi.style.backgroundColor='#FFFFFF'; lmi.style.color='';  lmi.firstChild.firstChild.style.color='';}
	obj.style.backgroundColor='#1076DC';
	obj.style.color='#FFFFFF'; lmi=obj;
	obj.firstChild.firstChild.style.color='#FFFFFF';
}

function itemover2(obj){
	if(lmi!=0){lmi.style.backgroundColor='#FFFFFF'; lmi.style.color='';  lmi.firstChild.firstChild.style.color='';} lmi=obj;
}

function loc(url){
	document.location.href=url;
}

function remove_selection(){
	if (window.getSelection) { window.getSelection().removeAllRanges(); }
	else if (document.selection && document.selection.clear)
	document.selection.clear();
}

function fclick1(src,id){
	var obj=document.getElementById('uc'+id);
	if(obj.value==1){
		obj.value=2;
		src.innerHTML='aa';
		src.title='без учёта регистра';
	} else {
		obj.value=1;
		src.innerHTML='aA';	
		src.title='с учётом регистра';
	}
}

function fclick2(src,id){
	var obj=document.getElementById('wh'+id);
	if(obj.value==3){
		obj.value=2;
		src.innerHTML="'a'";
		src.title='полное совпадение';
	} else if(obj.value==2){
		obj.value=1;
		src.innerHTML='.a.';
		src.title='частичное совпадение';
	} else {
		obj.value=3;
		src.innerHTML='<s>.a.</s>';
		src.title='не содержит (частично)';
	}
}

function fclick3(src,id){
	var obj=document.getElementById(id);
	if(obj.value==1){
		obj.value=0;
		src.value='A';
		src.title='по всем данным модуля';
	} else {
		obj.value=1;
		src.value='E';	
		src.title='по текущему разделу';
	}
}

if(typeof(jQuery)!='undefined'){

	jQuery.uaMatch = function( ua ) {
		ua = ua.toLowerCase();
	
		var match = /(chrome)[ \/]([\w.]+)/.exec( ua ) ||
			/(webkit)[ \/]([\w.]+)/.exec( ua ) ||
			/(opera)(?:.*version|)[ \/]([\w.]+)/.exec( ua ) ||
			/(msie) ([\w.]+)/.exec( ua ) ||
			ua.indexOf("compatible") < 0 && /(mozilla)(?:.*? rv:([\w.]+)|)/.exec( ua ) ||
			[];
	
		return {
			browser: match[ 1 ] || "",
			version: match[ 2 ] || "0"
		};
	};
	
	if ( !jQuery.browser ) {
		matched = jQuery.uaMatch( navigator.userAgent );
		browser = {};
	
		if ( matched.browser ) {
			browser[ matched.browser ] = true;
			browser.version = matched.version;
		}
	
		// Chrome is Webkit, but Webkit is also Safari.
		if ( browser.chrome ) {
			browser.webkit = true;
		} else if ( browser.webkit ) {
			browser.safari = true;
		}
	
		jQuery.browser = browser;
	}
	
	//jQuery.curCSS = jQuery.css;
	$.curCSS = function (element, attrib, val) {
	   $(element).css(attrib, val);
	   //alert(JSON.stringify(attrib)+' '+JSON.stringify(val));
	   //if(typeof(val)!='undefined') return $(element).css(attrib, val);
	   //else {
	   //	return $(element).css(attrib);
	   //}
	};
}

function check_fields(from){
	var msg='Недопустимое значение для специального имени';	
	if(from=='mod_col'){
		var f=$('input[name="col_sname"]');
	}
	var v=f.val().toLowerCase();
	if(v==''){
		alert(msg);
		return false;
	}			
	if(from=='mod_col'){
		var d="enable,disable,table,aurl,copy,copy_single,full_url,wayback,allway,wayback2,wayback3,wayback_url,allway3,allway_url,neighbors,neighbors2,neighbors_and_i,neighbors3,neighbors_wo_sub,user,group,set_user,sub,allsub,up,owner,ups,owners,col,sut,id,rid,eid,self,pos,geturl,major,ex,next,prev,next_simple,prev_simple,signal,prepare,linked,linked_same_module,linked_same_ex,linked_count,depend,highlight,highlight2,highlight_only,with,creation_date,edit_date".split(',');
	}
	for(var key in d){
		if(d[key]==v){
			alert(msg);
			return false;
		}
	}
	return true;
}