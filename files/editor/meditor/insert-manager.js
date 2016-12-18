var managers=Array();
var manager_ready=false;
var selectManager=0;
var selectFolder=0;
var selectButton=0;
var selectOption=0;
var pushedButton=0;

// VARS

var selectTable=0;
var currentTable=0;
var selectModule=0;
var selectModuleSname='';
var selectCol=0;
var selectPart=0;
var selectComponent=0;
var selectComponentType=0;
var isImport=0;
var isTableTop=0;
var isTableBottom=0;
var isTableEdit=0;
var isColEdit=0;
var isColShow=0;
var isColForm=0;
var isCron=0;
var isPartCase=0; //not used
var isPartDetect=0; //not used
var additional_vars=Array();

// CONSTANTS
var CMD_UNKNOWN = -1; 
var CMD_BASE = 0; //команды корня
var CMD_NONE = 1; //нет результата
var CMD_MIXED = 2; //смешанный результат, определять по содержимому DATA
var CMD_STRING = 3; //имеется ввиду любое value отличное от других типов
var CMD_ARRAY = 4; 
var CMD_MODULE = 5; 
var CMD_EX = 6; 
var CMD_TABLE = 7; 
var CMD_COL = 8; 
var CMD_ROW = 9; 
var CMD_USER = 10; 
var CMD_GROUP = 11; 
var CMD_PART = 12; 
var CMD_STAT = 13;  //набор функций для работы с статистикой
var CMD_CURTYPE = 14;  //результат, опирающийся на CUR_TYPE
var CMD_ZONE = 15; 
var CMD_FILE = 16;  //набор функций для работы с файловой системой
var CMD_GLOBAL = 17;  //набор функция для работы с глобальными значениями
var CMD_COL_FILE = 18; //для работы с файлами-столбцами
var CMD_COMPONENT = 19; //php-компонент, размещённый на физическом сервере
//var CMD_ROW_MODULE = 20;//для работы с модульным значением строки

// Дополнительные типы для переменных операторов (для визуального редактора)
var CMD_LOGICAL = 20;  //0, 1, true, false //для checkbox
var CMD_IF = 21;  //набор условий //сразу активируется секция "логическое условие" в редакторе значений
var CMD_FASTGET_IF = 22; //набор условий для fastget-функции
var CMD_FASTGET_ORDER = 23; //набор параметров для сортировки fastget-функции
var CMD_CMD = 24;  //команда, например cur.title.explode
var CMD_STATIC = 25; //статичное значение, определённое в поле visual->put[x]->static, при генерации указывается без скобочек (работает как чекбокс, если он отмечен - значение указывается, не может быть req)
var CMD_DOUBLE = 26; //вложенная группа из двух параметров, разделённых знаком :
var CMD_USER_GROUP = 27; //пользователь, группа или SNAME группы
var CMD_COL_TYPE = 28; //тип, соответствующий типу текущего поля
var CMD_TPL = 29; //шаблон

// Операции с fastget
var CMD_FIND=701;//fastget-find
var CMD_FIND_COUNT=702;//fastget-find-count
var CMD_FIND_WHERE=703;//fastget-find-where

// Мнимые подтипы для CMD_STRING
var STRING_COLNAMES = 101;  //массив sname полей, написанных в строчку, заключённых в одинарные или двойные кавычки (возможно не используется)
var STRING_COLNAME = 102;  //+ sname поля
var STRING_COLNAMES_SEARCH = 103; //массив sname полей, написанных в строчку, заключённых в одинарные или двойные кавычки, которые используются для поиска
var STRING_COLNAMES_ORDER = 104;  //массив sname полей, написанных в строчку, заключённых в одинарные или двойные кавычки, которые используются для сортировки
var STRING_SUBTABLE_NAMES = 105;  //sname подтаблицы
var STRING_PARAM_NAME = 106; //название переменной (пытается найти его среди текущей части, либо предложить пользователю свой вариант)
var STRING_EX_PARAM = 107; //sname одного из параметров модуля
var STRING_TABLE_NAME = 108; //+ sname одной из таблиц текущего модуля
var STRING_GROUP_NAME = 109; //++ sname группы
var STRING_PART_NAME = 110; //sname части
var STRING_FUNCTION_NAME = 111; //функция
var STRING_SHOW_NAME = 112; //отображение
var STRING_COMPONENT_NAME = 113; //компонент
var STRING_FORM_NAME = 114; //форма
var STRING_TABLE_NAME_ID = 115; //sname таблицы или её id (или сам объект таблица)
var STRING_FILENAME = 116; //путь до конкретного файла (или строка, генерирующая его) - здесь открывается файловый редактор
var STRING_MODULE_TABLE = 117; // module_sname.table_sname (для foreign_tables)
var STRING_DATE = 118; 
var STRING_TIME = 119; 
var STRING_DATETIME = 120; 
var STRING_NUM = 121; //число
var STRING_URL = 122; //URL
var STRING_BLOCK_NAME = 123; //block
var STRING_MODULE_SNAME = 124; //module sname
var STRING_COLNAME_ANY = 125; //colname из всех таблиц
var STRING_COLNAME_PARAM = 126; //colname for part param
var STRING_ROW_SELECT=127; // select rows from ajax query
var STRING_COLNAME_FIND=128; // выбор colname с поддержкой значений FASTGET

// Специальные типы операторов
var SP_NONE = 0; //обычные правила (указываются в массиве visual->put)
var SP_COMPONENT = 1; //правила для компонентов
var SP_PART = 2;  //правила для части (тоже, что и для компонентов, но с привязкой к модулю - не уверен надо ли это)
				//отличие от SP_COMPONENT также в том, что вместо привычного part.part1(x=1), может быть part.part1.sname (т.е. отсылка на обработчик части)
var SP_PREPARE = 3;  //устанавливает параметры обработки аргументов метода как для метода prepare (динамический набор столбцов)
var SP_FASTGET = 4;  // сложный разбор, следующий op может быть subtable_sname
//var SP_SC = 5; // разбор для команды SC, SC2 (следующий op = col_sname)
//var SP_ST = 6; //разбор для команды ST (следующий op = table_sname)
var SP_POINT = 5; //разбор, при котором следующий за . элемент попадает в put[0] (без кавычек)
var SP_SPACE = 6; //разбор для команд первого уровня, когда в случае next_operation=='  =  остаток помещается в PUT
var SP_PUP = 7; //разбор команд для PUP (с учётом его множественного повторения)
var SP_UP = 8; //разбор команд для UP (с учётом его множественного повторения)
var SP_AJAX = 9; //разбор команд для Ajax (далее переходит либо на SP COMPONENT, либо на SP PART
var SP_UNKNOWN = 10; //разбор неизвестного
var SP_DOUBLE_POINT = 11; //тоже, что SP_POINT, но используются две точки, т.е. не "op.x", а "op.x.y" (пример с foreign_tables)
var SP_IGNORE=12;//игнорировать эту команду при компиляции
var SP_EX_PARAM=13;//генерирование параметров экземпляра
var SP_ARRAY_INDEX=14;//индекс массива (имитация [] после предыдущего оператора)
var SP_WIDGET=15;//виджет
var SP_CACHE=16;//виджет
var SP_PART_PARAM=17;
var SP_ADD_COLS=18;

// Условия для употребления команд
var IF_CRON = 1; //отображается, если работа ведётся с частью, вызванной по таймеру
var IF_IMPORT = 2; //может ли часть работать с импортом
var IF_PART_CASE = 3; //работает только в условии для вызова части
var IF_TREE_NUMERIC = 4; //работает только внутри числовых переборов (переборов от min до max)
var IF_TABLE_BOTTOM = 5; //только для обработчика подвала таблицы
var IF_COW = 6; //если определён COW (работа ведётся в части с определнием URL)
var IF_CUR = 7; //если определён CUR (т.е. действие происходит в теле цикла)
var IF_MODULE = 8; //если часть работает в модуле
var IF_CUCOL = 9; //если определён CUCOL (работа ведётся в обработчике поля таблицы)
var IF_COL_FILE = 10; //только если текущее поле - файл
var IF_TABLE_TOP=11;
var IF_TABLE_EDIT=12;
var IF_COL_EDIT=13;
var IF_COL_FORM=14;
var IF_COL_SHOW=15;
var IF_PART_DETECT=16;
var IF_CUR_USER=17;
var IF_WIDGET=18;
var IF_PART_HAVE_PARAM=19;
var IF_PART_OF_MODULE=20;

// Типы входного объекта
var OB_NONE = 0;
var OB_TABLE = 1; // таблица
var OB_MODULE = 2; // модуль

// Группы
var GROUP_CONTROL=1001;
var GROUP_SYSTEM=1002;
var GROUP_SPECIFIC=1003;
var GROUP_OPERATIONS=1004;
var GROUP_FOR=1005;
var GROUP_URL=1006;
var GROUP_VARS=1007;
var GROUP_CONST=1008;

// Прочие
var DoubleQuotes="''";
var NoDefaultValue='-=|No Value|=-';

// Данные, загружаемые "на лету"
var x_exes=[];

// SUPPORT function
function set_scroll_focus(o,c,d){//o - Object, c - scroll Container, d - Debug container
	var x=c.get(0).scrollTop;
	
	var tmp_a=o.position();
	var tmp_b=c.position();
	if(!tmp_a) return false;
	if(!tmp_b) return false;
	
	var y=o.position().top-c.position().top+x;
	var z=c.height();
	//d.html('scroll: '+x+'; obj_y: '+y+'; h: '+z);
	if(y>z+x-50) c.get(0).scrollTop=y-o.height()+50-z;
	if(y<x/*+50*/) c.get(0).scrollTop=y-/*50-*/o.height();
}
function select_refocus(evt,type){
	//1 - down
	//2 - press
	//3 - up
	if(type==3 || type==2) return true;
	evt = evt || window.event;
	var key = evt.keyCode || evt.which;
	var focus=$(':focus');
	if(key==38){
		
	}
	if(key==40){
		
	}
	if(!focus.is('select')) return true;
	if(key!=13){
		focus.blur();
		return false;
	}
	return true;
}
function add_key_quotes(options){
	var result=[];
	for(var pos in options )for(var key in options[pos]){
		if(!result[pos]) result[pos]=[];
		if(key!='-') result[pos]["'"+key+"'"]=options[pos][key];
		else result[pos][key]=options[pos][key];
	}
	return result;
}
function add_option(options,key,value,index,type){
	var x=[];
	x[key]=[];
	x[key]['v']=value;
	if(index && type){
		x[key]['i']=index;
		x[key]['t']=type;
	}
	options[options.length]=x;
}
function find_cmd(type,name){
	for(var t in x_cmd)
		for(var p in x_cmd[t])
			for(var k in x_cmd[t][p])
				if(x_cmd[t][p][k].cmd_type==type && x_cmd[t][p][k].cmd==name)
					return x_cmd[t][p][k];
}
function double_input(id){
	var last=$('#i'+id+'-box');
	var html=last.html();
	var next_num=2;
	while($('#i'+id+'-box'+next_num).attr('id')){
		last=$('#i'+id+'-box'+next_num);
		next_num++;
	}
	html='<div id="i'+id+'-box'+next_num+'">'+html.split(id).join(id+'copy'+next_num)+'</div>';
	last.after(html);
}
function get_result_from_string(str){
	switch(str){
		case 'CMD_BASE': return CMD_BASE;
		case 'CMD_NONE': return CMD_NONE;
		case 'CMD_MIXED': return CMD_MIXED;
		case 'CMD_STRING': return CMD_STRING;
		case 'CMD_ARRAY': return CMD_ARRAY;
		case 'CMD_MODULE': return CMD_MODULE;
		case 'CMD_EX': return CMD_EX;
		case 'CMD_TABLE': return CMD_TABLE;
		case 'CMD_COL': return CMD_COL;
		case 'CMD_ROW': return CMD_ROW;
		case 'CMD_USER': return CMD_USER;
		case 'CMD_GROUP': return CMD_GROUP;
		case 'CMD_PART': return CMD_PART;
		case 'CMD_STAT': return CMD_STAT;
		case 'CMD_CURTYPE': return CMD_CURTYPE;
		case 'CMD_ZONE': return CMD_ZONE;
		case 'CMD_FILE': return CMD_FILE;
		case 'CMD_GLOBAL': return CMD_GLOBAL;
		case 'CMD_COL_FILE': return CMD_COL_FILE;
		case 'CMD_COMPONENT': return CMD_COMPONENT;
		default: return CMD_MIXED;
	}
}

// BIND buttons
if(!('up' in hotkeys)) hotkeys['up']=[];
hotkeys['up'][hotkeys['up'].length]=function(e,key){
	if(selectManager==0) return false;
	// ESC button
	if(key==27){
		if(pushedButton){
			pushedButton.onclick();
			pushedButton=0;
		} else if(selectManager!=0){
			if(selectFolder.prev){
				selectFolder.prev.restore_folder();
			} else selectManager.destroy();
			
		}
		return true;
	}
	// ENTER button
	if(key==13){
		if(selectFolder && selectFolder.have_type_select==2){
			//selectFolder.box.find('.select_type_box .select_box').focus();
		} else if(selectFolder && selectFolder.have_object_select==2){
			//selectFolder.box.find('.type_box:visible .select_box').focus();
		} else if(selectManager!=0 && selectFolder!=0 && selectButton!=0){
			if(!pushedButton) selectButton.onclick();
			else {
				if(pushedButton.selectButton==2) pushedButton.do_next();
				else if(pushedButton.selectButton==1 && pushedButton.btn_insert) pushedButton.do_insert();
				else if(pushedButton.selectButton==1 && pushedButton.btn_next) pushedButton.do_next();
				else if(pushedButton.selectOption && pushedButton.selectOption.input_properties) pushedButton.selectOption.input_properties.click();
			}
		}
		return true;
	}
}
if(!('down' in hotkeys)) hotkeys['down']=[];
hotkeys['down'][hotkeys['down'].length]=function(e,key){
	if(selectManager==0) return false;
	// DOWN
	if(key==40){
		if(selectFolder!=0){
			if(pushedButton==0){
				if(selectFolder.have_type_select==1 && selectFolder.have_object_select!=2 && selectButton==0){
					selectFolder.have_type_select=2;
					selectFolder.box.find('.select_type_box').css('border','1px solid #EEEEEE');
					set_scroll_focus(selectFolder.box.find('.select_type_box'),selectFolder.box,selectFolder.parent.title);
					selectFolder.box.find('.select_type_box .select_box').focus();
				} else if(selectFolder.have_type_select!=1 && selectFolder.have_object_select==1 && selectButton==0 && selectFolder.box.find('.type_box:visible')){
					if(selectFolder.have_type_select==2){
						selectFolder.have_type_select=1;
						selectFolder.box.find('.select_type_box').css('border','1px solid #FFFFFF');
					}
					selectFolder.have_object_select=2;
					selectFolder.box.find('.type_box:visible').css('border','1px solid #EEEEEE');
					selectFolder.box.find('.type_box:visible .select_box').focus();
					set_scroll_focus(selectFolder.box.find('.type_box:visible'),selectFolder.box,selectFolder.parent.title);
				}else if(selectButton==0){
					if(selectFolder.have_object_select==2){
						selectFolder.have_object_select=1;
						selectFolder.box.find('.type_box:visible').css('border','1px solid #FFFFFF');
						selectFolder.box.find('.type_box:visible .select_box').blur();
					}
					if(selectFolder.have_type_select==2){
						selectFolder.have_type_select=1;
						selectFolder.box.find('.select_type_box').css('border','1px solid #FFFFFF');
						selectFolder.box.find('.select_type_box .select_box').blur();
					}
					selectButton=selectFolder.buttons[0];
					selectButton.btn.addClass('m-button-select');
					set_scroll_focus(selectButton.btn,selectFolder.box,selectFolder.parent.title);
				} else if(selectButton && selectButton.next!=0){
					selectButton.btn.removeClass('m-button-select');
					selectButton=selectButton.next;
					selectButton.btn.addClass('m-button-select');
					//selectFolder.box.get(0).scrollTop=selectButton.btn.position().top;
					set_scroll_focus(selectButton.btn,selectFolder.box,selectFolder.parent.title);
				}
			} else {
				if(pushedButton.selectOption==0 && pushedButton.options.length>0){
					pushedButton.selectOption=pushedButton.options[0];
					pushedButton.selectOption.box.addClass('m-option-box-select');
					pushedButton.selectOption.input.focus();					
				} else if(pushedButton.selectOption && pushedButton.selectOption.next!=0){
					pushedButton.selectOption.box.removeClass('m-option-box-select');
					pushedButton.selectOption=pushedButton.selectOption.next;
					pushedButton.selectOption.box.addClass('m-option-box-select');
					pushedButton.selectOption.input.focus();
				} else {
					if(pushedButton.selectOption){
						pushedButton.selectOption.box.removeClass('m-option-box-select');
						pushedButton.selectOption.input.blur();
					}
					//select buttons
					if(pushedButton.selectButton==0){
						pushedButton.selectButton=1;
						if(pushedButton.btn_insert!=0){
							pushedButton.btn_insert.addClass('m-button-insert-hover');
							set_scroll_focus(pushedButton.btn_insert,selectFolder.box,selectFolder.parent.title);
						} else {
							pushedButton.btn_next.addClass('m-button-next-hover');
							set_scroll_focus(pushedButton.btn_next,selectFolder.box,selectFolder.parent.title);
						}
					}
					else if(pushedButton.selectButton==1 && pushedButton.btn_insert && pushedButton.btn_next){
						pushedButton.selectButton=2;
						pushedButton.btn_insert.removeClass('m-button-insert-hover');
						pushedButton.btn_next.addClass('m-button-next-hover');
						set_scroll_focus(pushedButton.btn_next,selectFolder.box,selectFolder.parent.title);
					}
				}
			}
		}
		e.stopPropagation();
		/*if(selectFolder && selectFolder.have_type_select==2){
		} else if(selectFolder && selectFolder.have_object_select==2){
		} else e.preventDefault();*/
		return true;
	}
	// UP
	if(key==38){
		if(selectFolder!=0){
			if(pushedButton==0){
				if(selectFolder.have_object_select==1 && selectFolder.have_type_select!=2 && selectFolder.box.find('.type_box:visible') && (selectButton==0 || (selectButton && !selectButton.prev))){
					selectFolder.have_object_select=2;
					selectFolder.box.find('.type_box:visible').css('border','1px solid #EEEEEE');
					set_scroll_focus(selectFolder.box.find('.type_box:visible'),selectFolder.box,selectFolder.parent.title);
					selectFolder.box.find('.type_box:visible .select_box').focus();
					if(selectButton){
						selectButton.btn.removeClass('m-button-select');
						selectButton=0;
					}
				} else if(selectFolder.have_object_select!=1 && selectFolder.have_type_select==1 && (selectButton==0 || (selectButton && !selectButton.prev))){
					selectFolder.have_type_select=2;
					selectFolder.box.find('.select_type_box').css('border','1px solid #EEEEEE');
					selectFolder.box.find('.select_type_box .select_box').focus();
					set_scroll_focus(selectFolder.box.find('.select_type_box'),selectFolder.box,selectFolder.parent.title);
					if(selectFolder.have_object_select==2){
						selectFolder.have_object_select=1;
						selectFolder.box.find('.type_box:visible').css('border','1px solid #FFFFFF');
					}
					selectFolder.box.find('.select_type_box').css('border','1px solid #EEEEEE');
				} else if(selectButton!=0 && selectButton.prev && selectButton.prev!=0 && selectFolder.buttons.length>0){
					selectButton.btn.removeClass('m-button-select');
					selectButton=selectButton.prev;
					selectButton.btn.addClass('m-button-select');
					//selectFolder.box.get(0).scrollTop=selectButton.btn.offset().top;
					set_scroll_focus(selectButton.btn,selectFolder.box,selectFolder.parent.title);
				} else if(selectButton==0){
					if(selectFolder.have_object_select==2 || selectFolder.have_type_select==2){
					} else if(selectFolder.buttons.count>0) {
						selectButton=selectFolder.buttons[0];
						selectButton.btn.addClass('m-button-select');
						set_scroll_focus(selectButton.btn,selectFolder.box,selectFolder.parent.title);
					}
				}
			} else {
				if(pushedButton.selectButton==2){
					pushedButton.selectButton=1;
					pushedButton.btn_next.removeClass('m-button-next-hover');
					pushedButton.btn_insert.addClass('m-button-insert-hover');
					set_scroll_focus(pushedButton.btn_insert,selectFolder.box,selectFolder.parent.title);
				} else if(pushedButton.selectButton==1){
					pushedButton.selectButton=0;
					if(pushedButton.btn_insert){
						pushedButton.btn_insert.removeClass('m-button-insert-hover'); 
						set_scroll_focus(pushedButton.btn_insert,selectFolder.box,selectFolder.parent.title);
					} else {
						pushedButton.btn_next.removeClass('m-button-next-hover'); 
						set_scroll_focus(pushedButton.btn_next,selectFolder.box,selectFolder.parent.title);
					}
					if(pushedButton.options.length>0){
						pushedButton.selectOption=pushedButton.options[pushedButton.options.length-1];
						pushedButton.selectOption.box.addClass('m-option-box-select');
						pushedButton.selectOption.input.focus();
					}
				} else if(pushedButton.selectOption && pushedButton.selectOption.prev){
					pushedButton.selectOption.box.removeClass('m-option-box-select');
					pushedButton.selectOption=pushedButton.selectOption.prev;
					pushedButton.selectOption.box.addClass('m-option-box-select');
					pushedButton.selectOption.input.focus();
				}
			}
		}
		e.preventDefault();
		/*if(selectFolder && selectFolder.have_type_select==2){
		} else if(selectFolder && selectFolder.have_object_select==2){
		} else e.preventDefault();*/
		return true;
	}
	return false;
}

// MANAGER constructor
function manager(start_type,insert_function,result_needed,nested_level,main_title,additional){

	// HTML insert
	if(typeof(im_id)=='undefined') im_id=0;
	else im_id++;
	$('body').append('<div class="m-shadow" id="m-shadow-'+im_id+'"><div class="m-box" id="m-box-'+im_id+'"></div></div>');
	this.old_selectManager=selectManager;
	this.old_selectFolder=selectFolder;
	this.old_selectButton=selectButton;
	this.old_selectOption=selectOption;
	this.old_pushedButton=pushedButton;
	if(additional) this.additional=additional;
	selectButton=0;
	selectFolder=0;
	selectOption=0;
	pushedButton=0;
	this.document_style_overflow=document.body.style.overflow;
	document.body.style.overflow = "hidden";
	this.shadow=$('#m-shadow-'+im_id);
	this.box=$('#m-box-'+im_id);
	this.id=im_id;
	this.insert_function=insert_function;
	if(start_type==undefined) start_type=0;
	if(!main_title) main_title='Вставить значение';
	this.box.append('<div class="m-top"><div class="m-top-right m-top-red f-left">X</div><div class="m-top-left m-top-title">'+main_title+'</div><!--<div class="m-top-right">Вставить</div>--></div>');
	this.title=this.box.find('.m-top-title');
	this.box.append('<div class="m-bar"></div>');
	this.bar=this.box.find('.m-bar');
	this.resultType=result_needed;
	if(!nested_level) this.nestedLevel=0;
	else {
		this.nestedLevel=nested_level;
		this.box.css('margin-left',(parseInt(this.box.css('margin-left'))+this.nestedLevel*10)+'px');
		this.box.css('margin-top',(parseInt(this.box.css('margin-top'))+this.nestedLevel*10)+'px');
	}
	this.redraw_box=function(margin){
		var x=(this.folders.length-1+margin);
		this.box.css('height',(501+x*31)+'px');
		this.box.css('margin-top',(-250-Math.floor(x*31/2))+'px');
	}
	
	// FORM components
	this.make_text=function(o,title,id,value,type,from_select){
		var t=0;
		var m=0;
		if(o.p && o.p.use_cur){
			t=o.parent.parent.seek_table();
			m=o.parent.parent.seek_module();
		}
		if(value==undefined) value='';
		if(value!=''/* && !isNumeric(value) && value.indexOf('.')==-1*/) value="'"+value+"'";
		var res='';
		res+='<div id="i'+id+'-box">';
			res+='<input type="text" class="m-option-input" id="'+id+'" value="'+value+'" OnClick="if(!this.value){this.value=DoubleQuotes; setCaretPosition(this,1);}">';
			res+='<input type="button" class="m-option-button" id="'+id+'_prop" value="..." OnClick="show_op_editor_text(\''+id+'\',0,'+type+','+(this.nestedLevel+1)+',\''+title.split('"').join('`')+'\''+(o.p && o.p.use_cur?',{use_cur:1,table:'+t+',module:'+m+'}':'')+')">';
		res+='</div>';
		if(!from_select){
			if(o.p.is_array){
				res+='<div align="right"><em class="link" OnClick="double_input(\''+id+'\')">добавить</em></div>';
			}			
		}
		have_properties=true;
		return res;
	}
	this.make_select=function(o,title,options,value,id,type,disable_insert_value,disable_multiply){
		if(value==undefined) value='';
		var res='';
		var opt='';
		for(var pos in options) for(var key in options[pos]){
			var op=options[pos][key];
			opt+='<option value="'+key+'"'+(op.i?' '+op.t+'="'+op.i+'"':'')+(key==value?' selected':'')+(key=='-'?' disabled':'')+'>'+op.v+'</option>';
		}
		if(opt!=''){
			var value_c=value;
			//if(value!='' && value.indexOf("'")==-1) value_c="'"+value+"'";
			if(typeof(disable_insert_value)=='undefined') opt+='<option value="insert_value">Ввести значение</option>';
			//var res='<select id="'+id+'" OnChange="if($(this).val()==\'insert_value\') {$(\'#'+id+'\').hide(); $(\'#'+id+'text\').show();}" OnKeyPress="return (pushedButton.selectOption && !pushedButton.selectButton && \'opt\'+pushedButton.selectOption.id==\''+id+'\');">'+res+'</select>';
			res+='<div id="i'+id+'-box">';			
				res+='<select id="'+id+'" OnChange="if($(this).val()==\'insert_value\') {$(\'#'+id+'\').hide(); $(\'#'+id+'text\').show();}" OnKeyDown="return select_refocus(event,1);" OnKeyPress="return select_refocus(event,2);" OnKeyUp="return select_refocus(event,3);">'+opt+'</select>';
				// text extension
				res+='<div id="'+id+'text" style="display: none;">'+this.make_text(o,title,id+'itext',value_c,type,1)+'</div>';
			res+='</div>';
		} else res=this.make_text(o,title,id,value_c,type,1);
		if(o.p.is_array){
			if(typeof(disable_multiply)=='undefined') res+='<div align="right"><em class="link" OnClick="double_input(\''+id+'\')">добавить</em></div>';
		}
		return res;
	}
	
	// FOLDER constructor
	this.folder=function(manager,start_type,prev,step){
	
		// HTML insert
		if(typeof(folder_id)=='undefined') folder_id=0;
		else folder_id++;
		if(typeof(step)=='undefined') step=0;
		manager.box.append('<div class="m-folder" id="m-folder-'+folder_id+'"></div>');
		this.parent=manager;
		if(typeof(this.parent.folders)!='undefined') this.parent_id=this.parent.folders.length; else this.parent_id=0;
		this.id=folder_id;
		this.box=$('#m-folder-'+folder_id);
		this.next=0;
		this.prev=0;
		this.old_selectFolder=selectFolder;
		this.old_selectButton=selectButton;
		this.old_selectOption=selectOption;
		this.old_pushedButton=pushedButton;
		selectOption=0;
		pushedButton=0;
		if(prev){
			this.prev=prev;
			this.prev.next=this;
		}
		this.type=start_type;
		this.selectedButton=0;
		this.selectedParam=[];
		this.inputObject=0; //table, module id
		this.inputObjectType=0; //OB constant
		this.selectedObject=0;
		this.selectedType=0;
		
		// MINI-bar create
		this.create_mini=function(selected_btn, title){
			var id=this.id;
			if(title.length>40) title=title.substr(0,40)+'…';
			var return_btn='<div class="m-top-right f-left">&#8592;</div>';
			this.parent.bar.append('<div class="m-mini-bar" id="mini-bar-'+id+'">'+return_btn+'<div class="m-top-title m-top-left">'+title+'</div></div>');
			this.bar=$('#mini-bar-'+id);
			this.bar.find('.m-top-right').get(0).onclick=this.restore_folder.bind(this);
			this.parent.redraw_box(1);
		}
		
		// RESTORE folder
		this.restore_folder=function(){
			this.next.destroy();
			this.box.show();
			this.bar.remove();
			this.next=0;
			this.parent.redraw_box(0);
		}
		
		// BUTTONS constructor
		this.button=function(folder,o,object,step){
			if(typeof(btn_id)=='undefined') btn_id=0;
			else btn_id++;
			if(typeof(step)=='undefined') step=0;
			this.title=o.title.charAt(0).toLocaleUpperCase()+o.title.substr(1);
			folder.box.append('<div class="m-button-box" id="btn-box-'+btn_id+'"><div class="m-button" id="btn-'+btn_id+'">'+this.title+'</div><div id="btn-bottom-'+btn_id+'" style="display: none;"></div></div>');
			this.parent=folder;
			this.op=o;
			this.object=object;
			this.step=step;
			this.expanded=false;
			this.onclick=function(){
				if(selectButton!=0 && selectButton!=this){
					if(selectButton.expanded) selectButton.btn_bottom.hide();
					selectButton.expanded=false;
					selectButton.btn.removeClass('m-button-select2');
					selectButton.btn.removeClass('m-button-select');
				}
				selectButton=this;
				selectButton.selectOption=0;
				if(this.action=='expand'){
					this.expanded=!this.expanded;
					this.btn_bottom.toggle();					
					if(this.expanded){
						this.btn.addClass('m-button-select2');
						this.expanded=true;
						pushedButton=this;
					} else {
						this.btn.removeClass('m-button-select2');
						this.expanded=false;
						if(pushedButton && pushedButton.selectOption) pushedButton.selectOption.box.removeClass('m-option-box-select');
						if(pushedButton){
							pushedButton.selectButton=0;
							if(pushedButton.btn_next) pushedButton.btn_next.removeClass('m-button-next-hover');
							if(pushedButton.btn_insert) pushedButton.btn_insert.removeClass('m-button-insert-hover');
						}
						pushedButton=0;						
					}
				}
				if(this.action=='insert') this.do_insert();
				if(this.action=='next') this.do_next();
			}
			this.btn=$('#btn-'+btn_id);
			if(this.op.float) this.btn.css('text-align',this.op.float);
			this.btn.get(0).onclick=this.onclick.bind(this);
			this.btn_box=$('#btn-box-'+btn_id);
			this.btn_bottom=$('#btn-bottom-'+btn_id);
			this.prev=prev_btn;
			if(prev_btn!=0) prev_btn.next=this;
			prev_btn=this;
			this.next=0;
			this.id=btn_id;
			var first=true;
			this.selectOption=0;
			this.selectButton=0;
			this.selectedButton=0;
			// ROW & EX AUTOLOAD FUNCTIONS
			this.check_and_load_exes=function(){
				for(var table in this.load_ex_on_expand)
				for(var tmp in this.load_ex_on_expand[table]){
					var e=this.load_ex_on_expand[table][tmp];
					if(!e.ex_load){
						if(!(table in x_exes)){
							x_exes[table]=load_json(jsq2+"/ajax?action=get_ex_for_table&table="+table);
						}
						e.ex_select.find('option').remove();
						if(!x_exes[table]){
							e.ex_select.append($('<option></option>').attr('value','0').text('Доступ к данным ограничен, либо данные отсутствуют'));
						} else {
							e.ex_select.append($('<option></option>').attr('value','0').text('Выберите экземпляр'));
							for(var key in x_exes[table]){
								e.ex_select.append($('<option></option>').attr('value',key).text(x_exes[table][key]));
							}
							//e.ex_select.append($('<option></option>').attr('value','insert_value').text('Ввести значение'));
						}
						e.ex_load=true;
					}
				}
			}
			this.load_rows=function(){
				//alert(this.ex_select.val()+' '+this.p.link);
				this.row_select_box.show();
				var tmp=load_json(jsq2+"/ajax?action=get_rows_for_table&table="+this.p.link+'&ex='+this.ex_select.val());
				this.row_select.find('option').remove();
				if(!tmp){
					this.row_select.append($('<option></option>').attr('value','0').text('Доступ к данным ограничен, либо данные отсутствуют'));
				} else for(var key in tmp){
					this.row_select.append($('<option></option>').attr('value',tmp[key][0]).text(tmp[key][1]));
				}
				this.row_select.append($('<option></option>').attr('value','insert_value').text('Ввести значение'));
			}
			// COMPILE AND INSERT
			this.do_insert=function(){
				// Initialize insert
				var folder=this.parent;
				folder.selectedButton=this;
				var manager=folder.parent;
				var result='';
				while(folder.prev) folder=folder.prev;
				while(folder){
					var b=folder.selectedButton;
					var o=b.op;
					if(o.special==SP_CACHE){
						result+='^';
						var c_type=b.options[0].get_value();
						var i_auth=b.options[1].get_value();
						var auto_del=b.options[2].get_value();
						var link_host=b.options[3].get_value();
						var c_time=b.options[4].get_value();
						var time_type=b.options[5].get_value();
						if(c_time!='') c_time='('+time_type+c_time+')';
						result+=c_type+i_auth+auto_del+link_host+c_time;
						folder=folder.next;
						continue;
					}
					if(o.cmd && !o.skip_folder && o.special!=SP_IGNORE){
						if(result!='') result+='.';
						// FASTGET EXCEPTIONS
						//if(b.options && b.options.length>0 && b.options[0].p) if(b.options[0].p.special=='fastget_ignore_all_ex' && b.options[0].get_value()) result+='a';
						if(b.options && b.options.length>0 && b.options[0].p) if(b.options[0].p.special=='fastget_ignore_ex' && b.options[0].get_value()==1) result+='e';
						if(b.options && b.options.length>0 && b.options[0].p) if(b.options[0].p.special=='fastget_ignore_ex' && b.options[0].get_value()==2) result+='a';
						if(b.options && b.options.length>1 && b.options[1].p) if(b.options[1].p.special=='fastget_ignore' && b.options[1].get_value()) result+='i';
						if(o.convert_to_dollar) result+='$';
						else result+=o.cmd;
						// MORE FASTGET EXCEPTIONS
						if(b.options && b.options.length>2 && b.options[2].p){
							if(b.options[2].p.special=='fastget_type' && b.options[2].get_value()){
								var restore_c=false;
								if(result.substr(result.length-1,1)=='c') restore_c=true;
								if(restore_c) result=result.substr(0,result.length-1);
								if(b.options[2].get_value()==1) result+='2';								
								if(b.options[2].get_value()==2) result+='3';
								if(restore_c) result+='c';
							}
						}
					}
					var param='';
					var set_sp_point=0;
					var pre_val='';
					//for(var k in o.put){
					var p_count=0;
					var margin=0;
					if(o.special==SP_FASTGET) margin=2;
					if(o.put) for(var i=0;i<o.put.length+margin;i++)/*if(!o.put[k].hidden)*/{
						//var p=o.put[k];
						var p=o.put[i-margin];
						var op=b.options[i];
						var val='';
						if(o.special==SP_FASTGET && i-margin==1 && op.get_value()==NoDefaultValue){
							set_sp_point=1;
							continue;
						}
						if(i-margin<0 || !p.hidden){
							if(op.p.special=='fastget_ignore' || op.p.special=='fastget_ignore_ex' || op.p.special=='fastget_ignore_all_ex' || op.p.special=='fastget_type') continue;
							val=op.get_value();
						}
						var req=p.req;
						if(!req && p.prevreq && pre_val) req=true;
						if(val=='' && p.hidden) val=p.default;
						if(val!='' && p.static) val=p.static;
						if(val=="'"+NoDefaultValue+"'" || val==NoDefaultValue) continue;
						pre_val=val;
						if(val=='' && !req && o.special!=SP_FASTGET) continue;
						if(p.special=='_'){
							if(val) result+='._';
							continue;
						}
						if(p.is_array && val){
							if(p.type==STRING_COLNAMES_ORDER){
								if(val.indexOf("'")!=-1) val=val.split("'").join('');
								if(val.indexOf(" ")!=-1) val=val.split(' ').join('');
								//if(val=="'"+NoDefaultValue+"'" || val==NoDefaultValue) continue;
								val="'"+val+"'";
							} else if(!p.ignore_array_in_combine) val='('+val+')';
						}
						if(((o.special==SP_POINT || o.special==SP_FASTGET || (o.special==SP_WIDGET && o.put)) && set_sp_point==0) || (o.special==SP_DOUBLE_POINT && set_sp_point<2)){
							set_sp_point++;
							if(val){
								if(result!='' && !o.convert_to_dollar && result.charAt(result.length-1)!='_') result+='.';
								result+=val;
							}
						} else {
							if(o.special==SP_SPACE){
								set_sp_point=true;
								if(result!='') result+=' ';
								result+=val;
							} else {
								// BASIC								
								if(param!=''){
									if(o.special==SP_FASTGET) param+=' ';
									else param+=', ';
								}
								if(p.sname){
									if(p.sname_quotes) param+="'"+p.sname+"'"+'=';
									else param+=p.sname+'=';
								}
								param+=val;
							}
						}
					}
					if(param){
						if(o.special==SP_ARRAY_INDEX) result+='['+param+']';
						else result+='('+param+')';
					}
					folder=folder.next;
					//alert(result);
				}
				//alert(result);
				insert_function(result);
				manager.destroy();
			}
			// NEXT FOLDER
			this.do_next=function(){
				// Inizialize new folder
				var folder=this.parent;
				var manager=folder.parent;
				folder.box.hide();
				folder.create_mini(this,this.title);
				folder.selectedButton=this;
				manager.folders[manager.folders.length]=new manager.folder(manager,this.op.result,folder);
			}
			// CREATE option
			this.option=function(parent, o, p){
				if(typeof(option_id)=='undefined') option_id=0;
				else option_id++;
				this.parent=parent;
				this.folder=parent.parent;
				this.manager=parent.parent.parent;
				this.id=option_id;
				this.p=p;
				this.op=o;
				//var oid=' id="btn-'+parent.id+'-opt'+option_id+'"';
				//var oid_n='btn-'+parent.id+'-opt'+option_id;				
				var oid_n='opt'+option_id;
				this.oid_n='opt'+option_id;
				var oid=' id="'+oid_n+'"';
				var data='';
				var options=[];//list options for select
				this.get_value=function(attr){
					var val=this.input.val();
					if(attr) val=this.input.find(':selected').attr(attr);
					if((!val || val=='insert_value') && this.input_text && this.input_text_box.css('display')!='none'){
						val=this.input_text.val();
					}
					
					// ARRAY COMPILATION
					var next_num=2;
					var id=this.input.attr('id');
					while($('#i'+id+'-box'+next_num).attr('id')){
						var new_input=$('#'+id+'copy'+next_num);
						var new_input_text=$('#'+id+'copy'+next_num+'itext');
						var new_input_text_box=$('#'+id+'copy'+next_num+'text');
						var new_val=new_input.val();
						if(attr) new_val=new_input.find(':selected').attr(attr);
						if((!new_val || new_val=='insert_value') && new_input_text && new_input_text_box.css('display')!='none'){
							new_val=new_input_text.val();
						}
						if(new_val=="'"+NoDefaultValue+"'" || new_val==NoDefaultValue){
							next_num++;
							continue;
						}
						if(val && new_val) val+=', ';
						if(new_val) val+=new_val;
						next_num++;
					}
					
					if(typeof(val)=='undefined') val='';
					if(this.input.attr('type')=='checkbox'){
						val=this.input.get(0).checked;
						if(this.input.attr('static')!='' && typeof(this.input.attr('static'))!='undefined'){
							if(val) val=this.input.attr('static');
							else val='';
						}
					}
					return val;
				}
				if((p.type==CMD_STATIC || p.type==CMD_LOGICAL) && !p.list){
					data='<label class="m-label"><input type="checkbox"'+oid+(p.static?' static="'+p.static+'"':'')+(p.default?' checked':'')+'> '+p.title+'</label>';
				} else if(p.type==STRING_GROUP_NAME){
					if(!p.req) add_option(options,NoDefaultValue,'Нет');
					for(var v in x_module.groups){
						var l=x_module.groups[v];
						add_option(options, l.group_sname, l.group_name,(p.set_attr?l.auth_id:0),'group');
						//options[l.group_sname]=l.group_name;
					}
					for(var m in x_module.foreign){
						for(var v in x_module.foreign[m].groups){
							var l=x_module.foreign[m].groups[v];
							//options[l.group_sname]=l.group_name+' ('+x_module.foreign[m].name+')';
							add_option(options, l.group_sname, l.group_name+' ('+x_module.foreign[m].name+')',(p.set_attr?l.auth_id:0),'group');
						}
					}
					if(p.in_quotes) options=add_key_quotes(options);
					data=p.title+':<br>'+this.manager.make_select(this,p.title,options,p.default,oid_n,p.type);
				} else if(p.type==STRING_PART_NAME){
					if(!p.req) add_option(options,NoDefaultValue,'Нет');
					this.echo_parts=function(parts,owner,options,set_attr,space){
						if(parts) for(var v in parts[owner]){
							var l=parts[owner][v];
							add_option(options, l.part_sname, space+l.part_name,(set_attr?l.part_id:0),'part');
							this.echo_parts(parts,l.part_id,options,set_attr,space+'- ');
						}
					}
					this.echo_parts(x_module.parts,0,options,p.set_attr,'');
					if(p.in_quotes) options=add_key_quotes(options);
					data=p.title+':<br>'+this.manager.make_select(this,p.title,options,p.default,oid_n,p.type);
				} else if(p.type==STRING_TABLE_NAME_ID || p.type==STRING_TABLE_NAME){
					if(!p.req) add_option(options, NoDefaultValue,'Нет');//options['']='Нет';
					for(var v in x_module.tables){
						var l=x_module.tables[v];
						//options[l.table_sname]=l.table_name;
						add_option(options, l.table_sname, l.table_name,(p.set_attr?l.table_id:0),'table');
					}
					for(var m in x_module.foreign){
						for(var v in x_module.foreign[m].tables){
							var l=x_module.foreign[m].tables[v];
							//options[l.table_sname]=l.table_name+' ('+x_module.foreign[m].name+')';
							add_option(options, l.table_sname, l.table_name+' ('+x_module.foreign[m].name+')',(p.set_attr?l.table_id:0),'table');
						}
					}
					if(p.in_quotes) options=add_key_quotes(options);
					data=p.title+':<br>'+this.manager.make_select(this,p.title,options,p.default,oid_n,p.type);
				} else if(p.type==STRING_SUBTABLE_NAMES){
					if(!p.req) add_option(options, NoDefaultValue,'Нет');
					var t=this.parent.parent.seek_table();
					t=this.parent.parent.get_table_instance(t);
					var need_all_tables=true;
					if(t && t.subtables){
						for(var key in t.subtables){
							need_all_tables=false;
							var st=this.parent.parent.get_table_instance(t.subtables[key]);
							if(st) add_option(options, st.table_sname, st.table_name, st.table_id,'table');
						}
					}
					if(need_all_tables){
						for(var v in x_module.tables){
							var l=x_module.tables[v];
							add_option(options, l.table_sname, l.table_name, l.table_id,'table');
						}
						for(var m in x_module.foreign){
							for(var v in x_module.foreign[m].tables){
								var l=x_module.foreign[m].tables[v];
								add_option(options, l.table_sname, l.table_name+' ('+x_module.foreign[m].name+')', l.table_id,'table');
							}
						}
					}
					if(p.in_quotes) options=add_key_quotes(options);
					data=p.title+':<br>'+this.manager.make_select(this,p.title,options,p.default,oid_n,p.type);
				} else if(p.type==STRING_COLNAME || p.type==STRING_COLNAMES_ORDER || p.type==STRING_COLNAME_ANY || p.type==STRING_COLNAME_PARAM || p.type==STRING_COLNAME_FIND){
					if(p.type==STRING_COLNAMES_ORDER) p.is_array=1;
					if(!p.req) add_option(options, NoDefaultValue, 'Нет');//options['']='Нет';
					var t=this.parent.parent.seek_table();
					if(!t || p.type==STRING_COLNAME_ANY){
						for(var v in x_module.tables){
							//options[]='Таблица "'+x_module.tables[v].name+'"';
							add_option(options, '-', 'Таблица "'+x_module.tables[v].table_name+'"');
							for(var z in x_module.tables[v].cols){
								var l=x_module.tables[v].cols[z];
								//options[l.col_sname]=l.col_name;
								add_option(options, l.col_sname, l.col_name,(p.set_attr?l.col_id:0),'col');
							}
						}
						for(var m in x_module.foreign){
							for(var v in x_module.foreign[m].tables){
								//options[]='Таблица "'+x_module.tables[v].name+'" ('+x_module.foreign[m].name+')';
								add_option(options, '-', 'Таблица "'+x_module.foreign[m].tables[v].table_name+'" ('+x_module.foreign[m].name+')');
								for(var z in x_module.foreign[m].tables[v].cols){
									var l=x_module.foreign[m].tables[v].cols[z];
									//options[l.col_sname]=l.col_name;
									add_option(options, l.col_sname, l.col_name,(p.set_attr?l.col_id:0),'col');
								}
							}
						}
					} else {
						var t=this.parent.parent.get_table_instance(t);
						if(t){
							for(var z in t.cols){
								var l=t.cols[z];
								add_option(options, l.col_sname, l.col_name,(p.set_attr?l.col_id:0),'col');
							}
						}
						if(p.type==STRING_COLNAME_FIND){
							add_option(options, 'id', 'Идентификатор',0,'col');
							add_option(options, 'pos', 'Позиция',0,'col');
							add_option(options, 'owner', 'Родитель',0,'col');
							add_option(options, 'user', 'Пользователь-владелец',0,'col');
							add_option(options, 'major', 'Главное поле',0,'col');
							add_option(options, 'creation_date', 'Время создания',0,'col');
							add_option(options, 'modified_date', 'Время изменения',0,'col');
						}
					}
					if(p.in_quotes || p.type==STRING_COLNAME_PARAM) options=add_key_quotes(options);
					data=p.title+':<br>'+this.manager.make_select(this,p.title,options,p.default,oid_n,p.type);
				} else if(p.type==STRING_MODULE_SNAME){
					if(!p.req) add_option(options, NoDefaultValue, 'Нет');//options['']='Нет';
					if(x_module){
						var l=x_module;
						//options[l.sname]=l.name;
						add_option(options, l.sname, l.name,(p.set_attr?l.id:0),'module');
					}
					for(var m in x_module.foreign){
						var l=x_module.foreign[m];
						//options[l.sname]=l.name;
						add_option(options, l.sname, l.name,(p.set_attr?l.id:0),'module');
					}
					if(p.in_quotes) options=add_key_quotes(options);
					data=p.title+':<br>'+this.manager.make_select(this,p.title,options,p.default,oid_n,p.type);
				} else if(p.type==STRING_ROW_SELECT){
					data=p.title;
					add_option(options,'','Ожидайте загрузку');
					data+='<div>'+this.manager.make_select(this, p.title, options, p.default, oid_n+'ex', p.type, 1, 1)+'</div>';
					data+='<div style="display: none;" id="'+oid_n+'-row-select">'+this.manager.make_select(this, p.title, options, p.default, oid_n, p.type, 1)+'</div>';					
				} else if(p.list){
					for(var v in p.list){
						var l=p.list[v];
						//options[v]=l;
						add_option(options, v, l);
					}
					data=p.title+':<br>'+this.manager.make_select(this, p.title,options,p.default,oid_n,p.type);
				} else {
					data=p.title+':<br>'+this.manager.make_text(this, p.title,oid_n,p.default,p.type);
				}
				parent.sub_box.append('<div class="m-button-option" id="'+oid_n+'_box">'+data+'</div>');
				if(p.type==STRING_ROW_SELECT){				
					this.ex_select=$('#'+oid_n+'ex');
					this.row_select=$('#'+oid_n);
					this.row_select_box=$('#'+oid_n+'-row-select');
					var ex_on_change=this.parent.load_rows.bind(this);
					$('#'+oid_n+'ex').get(0).onchange=ex_on_change;
					this.ex_load=false;
					if(!this.parent.load_ex_on_expand) this.parent.load_ex_on_expand=[];
					if(!(p.link in this.parent.load_ex_on_expand)) this.parent.load_ex_on_expand[p.link]=[];
					this.parent.load_ex_on_expand[p.link][this.parent.load_ex_on_expand[p.link].length]=this;
					this.parent.check_and_load_exes();
				}
				this.box=$('#'+oid_n+'_box');
				this.input=$('#'+oid_n);
				this.input_text=$('#'+oid_n+'itext');
				this.input_text_box=$('#'+oid_n+'text');
				this.prev=0;
				this.next=0;
				this.input_properties=$('#'+oid_n+'_prop')
				if(parent.options.length>0){
					this.prev=parent.options[parent.options.length-1];
					parent.options[parent.options.length-1].next=this;
				}
			}
			this.options=[];
			if(o.put && o.put.length>0){				
				for(var key in o.put)if(!o.put[key].hidden){
					if(first){
						this.btn_bottom.append('<div class="m-button-sub" id="btn-sub-'+btn_id+'"></div>');
						this.sub_box=$('#btn-sub-'+btn_id);
 						if(typeof(o.text)!='undefined' && o.text!='') this.sub_box.append('<div class="m-button-text">'+o.text+'</div>');
						first=false;
						if(o.special==SP_FASTGET){
							//this.options[this.options.length]=new this.option(this,o,{'title':'Игнорировать экземпляр модуля','type':CMD_LOGICAL,'default':0,'special':'fastget_ignore_all_ex'});
							//this.options[this.options.length]=new this.option(this,o,{'title':'Игнорировать экземпляр таблицы','type':CMD_LOGICAL,'default':0,'special':'fastget_ignore_ex'});
							this.options[this.options.length]=new this.option(this,o,{'title':'Игнорировать экземпляр','type':CMD_STATIC,'default':0,'special':'fastget_ignore_ex','list':['Нет','Экземпляр таблицы','Экземпляр модуля']});
							this.options[this.options.length]=new this.option(this,o,{'title':'Игнорировать родителей','type':CMD_LOGICAL,'default':0,'special':'fastget_ignore'});
							this.options[this.options.length]=new this.option(this,o,{'title':'Активность объектов','type':CMD_STATIC,'default':0,'special':'fastget_type','list':['Только активные','Только не активные','Все']});
						}
					}
					this.options[this.options.length]=new this.option(this,o,o.put[key]);
					//o.put[key].option=this.options[this.options.length-1];
				}
			} else if(typeof(o.text)!='undefined' && o.text!=''){
				this.btn_bottom.append('<div class="m-button-sub" id="btn-sub-'+btn_id+'"></div>');
				this.sub_box=$('#btn-sub-'+btn_id);
				if(typeof(o.text)!='undefined' && o.text!='') this.sub_box.append('<div class="m-button-text" style="border-bottom: 0px; padding-bottom: 0px;">'+o.text+'</div>');
			}
			//var can_insert=(!this.op.disable_insert && (!this.parent.parent.resultType || o.result==this.parent.parent.resultType));
			var can_insert=(!this.op.disable_insert/* && (!this.parent.parent.resultType || this.parent.parent.resultType==CMD_BASE || o.result!=CMD_NONE)*/);
			var can_next=(o.result!=CMD_NONE);
			if(can_insert) this.action='insert';
			if(can_next) this.action='next';
			if(!first || (can_insert && can_next)){
				this.action='expand';
				if(!can_insert && !can_next && !first) can_insert=true;
				var width='281px;';
				var btn_insert='';
				var btn_next='';
				if(can_insert && can_next) width='135px;';
				if(can_insert) btn_insert='<div class="m-button-insert" id="insert-'+btn_id+'" style="width: '+width+'">&#8595;</div>';
				if(can_next) btn_next='<div class="m-button-next" id="next-'+btn_id+'" style="width: '+width+(can_insert?'border-left: 1px solid #999999;':'')+'">&#8594;</div>';
				this.btn_bottom.append('<div class="m-button-control" id="btn-ctrl-'+btn_id+'">'+btn_insert+btn_next+'</div>');
				if(can_insert){
					this.btn_insert=$('#insert-'+btn_id);
					this.btn_insert.get(0).onclick=this.do_insert.bind(this);
				} else this.btn_insert=0;
				if(can_next){
					this.btn_next=$('#next-'+btn_id);
					this.btn_next.get(0).onclick=this.do_next.bind(this);
				} else this.btn_next=0;
			}
			this.destroy=function(){
				this.btn_box.remove();
			}
			
			//this.mouseover=function(){if(selectButton!=0) selectButton.btn.removeClass('m-button-select'); selectButton=this;}
			//this.btn.get(0).onmouseover=this.mouseover.bind(this);
		}
		var prev_btn=0;
		this.get_module_instance=function(m){
			if(m && !m.id){
				if(m==x_module.id) return x_module;
				else if(x_module && x_module.foreign && m in x_module.foreign) return x_module.foreign[m];
				else return false;
			} return m;
		}
		this.get_table_instance=function(t){
			if(t && !t.table_id){
				if(t in x_module.tables) return x_module.tables[t];
				else if(t in x_module.foreign_table) return x_module.foreign[x_module.foreign_table[t]].tables[t];
				else {
					for(var m in x_module.foreign)
						if(t in x_module.foreign[m].tables) return x_module.foreign[m].tables[t];
				}
			} return t;
		}
		this.get_part_instance=function(p){
			var result=false;
			if(x_module){
				for(var key_owner in x_module.parts){
					if(p in x_module.parts[key_owner]){
						result=x_module.parts[key_owner][p];
						break;
					}
				}
			}
			if(!result){
				for(var key_proc in x_component){
					for(var key_cat in x_component[key_proc])
						if('parts' in x_component[key_proc][key_cat])
							if(p in x_component[key_proc][key_cat]['parts']){
								result=x_component[key_proc][key_cat]['parts'][p];
								break;
							}
					if(result) break;
				}
			}
			return result;
		}
		this.seek_module=function(start_folder){
			if(typeof(start_folder)=='undefined') var tp=this.prev;
			else var tp=start_folder;
			var module=0;
			while(tp){
				if(tp.selectedButton.op.module){
					module=tp.selectedButton.op.module;
					break;
				}
				if((tp.selectedButton.op.result==CMD_MIXED && (tp.selectedType==CMD_EX || tp.selectedType==CMD_MODULE) || (tp.inputObjectType==CMD_EX || tp.inputObjectType==CMD_MODULE)) && tp.selectedObject){
					module=tp.selectedObject;
					break;
				}
				if(tp.selectedButton.options){
					for(var key in tp.selectedButton.options){
						var o=tp.selectedButton.options[key];
						if(o.get_value('module')){
							module=o.get_value('module');
							break;
						}
					}
					if(module!=0) break;
				}
				tp=tp.prev;
			}
			if(/*!module && */(this.from_part || this.selectedType==CMD_EX || this.selectedType==CMD_MODULE || this.inputObjectType==CMD_EX || this.inputObjectType==CMD_MODULE) && this.selectedObject) module=this.selectedObject;
			return module;
		}
		this.seek_table=function(start_folder){
			if(typeof(start_folder)=='undefined') var tp=this.prev;
			else var tp=start_folder;
			var table=0;
			while(tp){
				if(tp.selectedButton.op.table){
					table=tp.selectedButton.op.table;
					break;
				}
				if((tp.selectedButton.op.result==CMD_MIXED && (tp.selectedType==CMD_ROW || tp.selectedType==CMD_TABLE) || (tp.inputObjectType==CMD_ROW || tp.inputObjectType==CMD_TABLE)) && tp.selectedObject){
					table=tp.selectedObject;
					break;
				}
				if(tp.selectedButton.options){
					for(var key in tp.selectedButton.options){
						var o=tp.selectedButton.options[key];
						if(/*o.p.type==STRING_SUBTABLE_NAMES && */o.get_value('table') && isNumeric(o.get_value('table'))){
							table=o.get_value('table');
							break;
						}
					}
					if(table!=0) break;
				}
				//if(tp.selectedButton.op.cmd=='major' && tp.selectedButton.op.type==CMD_TABLE){
				//	alert('123');
				//}
				tp=tp.prev;
			}
			var needType=this.selectedType;
			// GROUP TYPES
			if(needType>1000){
				for(var pos in x_cmd[needType]){
					for(var key in x_cmd[needType][pos]){
						needType=x_cmd[needType][pos][key].cmd_type;
						break;
					}
					if(needType<1000) break;
				}
			}
			if((needType==CMD_ROW || needType==CMD_TABLE || this.inputObjectType==CMD_ROW || this.inputObjectType==CMD_TABLE) && this.selectedObject) table=this.selectedObject;
			if(typeof(table)=='string' && table.indexOf('.')){
				var tmp=table.split('.');
				table=tmp[tmp.length-1];
			}
			return table;
		}
		this.seek_part=function(start_folder){
			if(typeof(start_folder)=='undefined') var tp=this.prev;
			else var tp=start_folder;
			var part=0;
			while(tp){
				if(tp.selectedButton.op.part){
					part=tp.selectedButton.op.part;
					break;
				}
				tp=tp.prev;
			}
			if(part==0) return selectPart;
			return part;
		}
		this.redraw_buttons=function(start_type,start_object){
			prev_btn=0;
			// SET selected
			this.selectedType=start_type;
			this.selectionObject=start_object;
			this.selectedObject=start_object;
			this.rewrite_start_type_for_select=0;
			this.rewrite_select_obj_for_select=0;
			// REMOVE old buttons
			if(this.buttons){
				for(var key in this.buttons){
					this.buttons[key].destroy();
				}
			}
			this.buttons=[];
			// CREATE buttons
			var x_cmd_tmp=x_cmd;
			if(!(start_type in x_cmd_tmp) && /*this.prev.selectButton.op.work_on*/start_type in x_cmd.work_on){
				//alert(this.prev.selectButton.op.work_on);
				x_cmd_tmp=x_cmd.work_on;
			}
			
			// ADDITIONAL BUTTONS TOP
			if(!this.prev && additional_vars.length>0){
				for(key in additional_vars){
					var av_name=additional_vars[key];
					if(av_name.indexOf('|')!='1'){
						var tmp=av_name.split('|');
						var add_var='$';
						if(tmp[0].indexOf('glob.')!='-1') add_var='';
						var op={
							cmd: add_var+tmp[0],
							title: 'Переменная "'+tmp[1]+'"',
							result: get_result_from_string(tmp[2])
						};
						if(tmp[3]) op.table=tmp[3];
					} else {
						var add_var='$';
						if(av_name.indexOf('glob.')!='-1') add_var='';
						var op={
							cmd: add_var+av_name,
							title: 'Переменная "'+av_name+'"',
							result: CMD_MIXED
						};
					}
					this.buttons[this.buttons.length]=new this.button(this,op,0,0);
				}
			}
			if(!this.prev && this.parent.additional && this.parent.additional.use_cur){
				var op={
					cmd: 'cur',
					title: 'Текущий объект (элемент цикла/фильтра)',					
					result: CMD_ROW
				};
				if(this.parent.additional.module) op.module=this.parent.additional.module;
				if(this.parent.additional.table) op.table=this.parent.additional.table;
				this.buttons[this.buttons.length]=new this.button(this,op,0,0);
			}
			if(this.prev && this.prev.selectedButton){
				var po=prev.selectedButton.op;
				if(po.from=='up'){
					this.buttons[this.buttons.length]=new this.button(this,po.repeat,0,0);
					if(po.repeat.cmd=='up'){
						this.buttons[this.buttons.length]=new this.button(this,find_cmd(CMD_BASE,'index'),0,0);
						this.buttons[this.buttons.length]=new this.button(this,find_cmd(CMD_BASE,'var'),0,0);
						this.buttons[this.buttons.length]=new this.button(this,find_cmd(CMD_BASE,'value'),0,0);
					}
					if(po.repeat.cmd=='pup'){
						this.buttons[this.buttons.length]=new this.button(this,{
							'cmd':'cow',
							'title':'Текущий объект части',
							'result':CMD_ROW
						},0,0);
						this.buttons[this.buttons.length]=new this.button(this,{
							'cmd':'table',
							'title':'Связанная с частью таблица',
							'result':CMD_TABLE
						},0,0);
						this.buttons[this.buttons.length]=new this.button(this,{
							'cmd':'cex',
							'title':'Текущий экземпляр модуля',
							'result':CMD_EX
						},0,0);
						this.buttons[this.buttons.length]=new this.button(this,{
							'cmd':'module',
							'title':'Текущий модуль',
							'result':CMD_MODULE
						},0,0);
						this.buttons[this.buttons.length]=new this.button(this,{
							'cmd':'part',
							'title':'Методы части',
							'result':CMD_PART
						},0,0);
					}
				}
			}
			
			//var m_for_ex_param=0;
			if(start_type in x_cmd_tmp){
				for(var pos in x_cmd_tmp[start_type])
				for(var key in x_cmd_tmp[start_type][pos]){
					var o=x_cmd_tmp[start_type][pos][key];
					if(!o) continue;
					// CHECK FOR EMPTY GROUP
					if(o.is_group){
						var valid=false;
						for(var tmp_pos in x_cmd[o.result])for(var tmp in x_cmd[o.result][tmp_pos]){
							var tmp_o=x_cmd[o.result][tmp_pos][tmp];
							if(!tmp_o.hidden && (tmp_o.result!=CMD_NONE || !this.parent.resultType)){
								valid=true;
								break;
							}
						}
						if(!valid) continue;
					}
					// ECHO BUTTON
					if(
						!o.hidden
						&& (o.result!=CMD_NONE || !this.parent.resultType)	// except buttons without result, if result needed
					){
						// ADDITIONAL OBJECT DETECTION
						if(!start_object && this.prev && this.prev.selectedButton){
							var bo=this.prev.selectedButton.op;
							if(bo.table) start_object=bo.table;
							else if(bo.module) start_object=bo.module;
							if(start_object && start_object.id) start_object=start_object.id;
						}
						if(o.object_type){
							this.inputObjectType=o.object_type;
						}
						// DISABLE INSERT for some types
						if(o.special && !o.step && o.special==SP_COMPONENT) o.disable_insert=true;
						// DISABLE TOP SELECT for some types
						if(o.special && !o.step && o.special==SP_COMPONENT) o.result=-1;
						if(o.special && !o.step && o.special==SP_PART_PARAM) o.result=-1;
						if(o.special && !o.step && o.special==SP_WIDGET) o.result=-1;
						if((o.special && !o.step && o.special==SP_PART) || (o.special && !o.step && o.special==SP_EX_PARAM)){
							o.module=this.seek_module();
							o.result=-1;
							//o.module=start_object;
						}
						
						var allow=true;						
						
						// SET SPECIFIC PARAMS
						if((o.cmd=='up' && o.cmd_type!=CMD_ROW) || o.cmd=='pup'){
							o.from='up';
							o.repeat=o;
						}
						if(o.cmd=='major' && (o.cmd_type==CMD_MODULE || o.cmd_type==CMD_EX)){
							o.module=this.get_module_instance(this.seek_module());
							if(!o.module.major || o.module.major==0) allow=false;
							else {
								o.table=o.module.major;								
							}
						}
					
						if(!o.title && !o.anyway_include){							
							// GENERATE BUTTONS FOR ROWS
							if(o.cmd_type==CMD_ROW && start_object){
								var t=0;
								if(x_module.tables && start_object in x_module.tables) t=x_module.tables[start_object];
								else for(var key in x_module.foreign) if(x_module.foreign[key].tables[start_object]){
									t=x_module.foreign[key].tables[start_object];
									break;
								}
								if(t){
									for(var key in t.cols){
										var ct=t.cols[key];
										var to={
											'title': 'Поле "'+ct.col_name+'"',
											'from': 'col',
											'cmd': ct.col_sname,
											'result': CMD_MIXED
										};
										if(ct.col_type==0) to.result=CMD_STRING;
										if(ct.col_type==1 && ct.col_link2==0) to.result=CMD_ROW;
										if(ct.col_type==1 && ct.col_link2==1){ to.result=CMD_ARRAY; to.result_long=CMD_ROW; }
										if(ct.col_type==2) to.result=CMD_STRING;
										if(ct.col_type==3){
											to.result=/*CMD_COL_FILE*/STRING_FILENAME;
											to.cmd_type=CMD_STRING;
										}
										if(ct.col_type==4){
											if(ct.module_url!=0) to.result=CMD_EX;//CMD_ROW_MODULE;//CMD_EX;
											else to.result=CMD_PART;
										}
										if(ct.col_type==5) to.result=CMD_USER;
										if(ct.col_type==6) to.result=CMD_STRING;
										//if(ct.col_type==7) to.result=CMD_GROUP;
										if(ct.col_type==1) to.table=ct.col_link;
										if(ct.col_type==1 && ct.col_deep!=0) to.table=ct.col_deep;
										this.buttons[this.buttons.length]=new this.button(this,to,ct,0);
									}
									// SUBTABLES
									for(var key in t.subtables){
										var ct=t.subtables[key];
										var oct=ct;
										var cm=0;
										if(ct in x_module.tables){
											cm=x_module;
										} else {
											cm=x_module.foreign[x_module.foreign_table[ct]];
										}
										ct=cm.tables[ct];
										var to={
											'title': 'Подтаблица "'+ct.table_name+'"',
											'result': CMD_TABLE,
											'cmd': ct.table_sname,
											'table': ct.table_id,
											'module': cm
										};
										this.buttons[this.buttons.length]=new this.button(this,to,ct,0);
									}									
								}
							}
							// GENERATE BUTTONS FOR EX							
							if(o.cmd_type==CMD_EX && start_object){
								var m=0;
								if(start_object==x_module.id) m=x_module;
								else if(start_object in x_module.foreign) m=x_module.foreign[start_object];
								if(m){
									//m_for_ex_param=m;
									for(var key in m.tables){
										var ct=m.tables[key];
										var to={
											'title': 'Таблица "'+ct.table_name+'"',
											'result': CMD_TABLE,
											'cmd': ct.table_sname,
											'table': ct.table_id
										};
										this.buttons[this.buttons.length]=new this.button(this,to,ct,0);
									}
								}
							}
							// GENERATE BUTTONS FOR MODULES
							// PARTS & EXS
							if(o.cmd_type==CMD_MODULE && start_object){
								// PARTS MUST HAVE HIERARCHY (ALSO PARTS FROM FOREIGN MODULES)
								// EXES MUST HAVE PUT SELECTBOX WITH EX NAME
								
								// PARTS SHOWN AS
								// PART OWNER
								// -- PART CHILD
								// -- -- PART SUB CHILD
								// -- PART CHILD 2
								// PART 2
								var exes=[];
								var cm=x_module.exs;
								if(x_module.foreign) if(start_object in x_module.foreign) cm=x_module.foreign[start_object].exs;
								for(var key in cm){
									var ce=cm[key];
									exes[ce.ex_sname]=ce.ex_name;
								}
								var to={
									'title': 'Экземпляр',
									'result': CMD_EX,
									'special': SP_POINT,
									'put': [{
										'title':'Выберите экземпляр',
										'list':exes,
										'req':1
									}]
								};
								this.buttons[this.buttons.length]=new this.button(this,to,0,0);
							}
						} else {
							if(o.condition){
								if(o.condition==IF_COW){
									if(!selectTable) allow=false;
									else {
										o.table=selectTable;
										o.module=selectModule;
									}
								}
								if(o.condition==IF_CUCOL){
									if(!selectCol) allow=false;
									else o.col=selectCol;
								}
								if(o.condition==IF_MODULE){
									if(!selectModule) allow=false;
									else o.module=selectModule;
								}
								if(o.condition==IF_IMPORT && !isImport) allow=false;
								if(o.condition==IF_CRON && !isCron) allow=false;
								if(o.condition==IF_TABLE_BOTTOM && !isTableBottom) allow=false;
								if(o.condition==IF_TABLE_TOP && !isTableTop) allow=false;
								if(o.condition==IF_TABLE_EDIT && !isTableEdit) allow=false;
								if(o.condition==IF_COL_EDIT && !isColEdit) allow=false;
								if(o.condition==IF_COL_FORM && !isColForm) allow=false;
								if(o.condition==IF_COL_SHOW && !isColShow) allow=false;
								if(o.condition==IF_PART_DETECT && !isPartDetect) allow=false;
								if(o.condition==IF_PART_CASE && !isPartCase) allow=false;
								if(o.condition==IF_CUR_USER){
									if(this.prev && !this.prev.selectedButton.op.is_cur_user) allow=false;
								}
								if(o.condition==IF_WIDGET && (!x_widget || x_widget.length==0)) allow=false;
								if(o.condition==IF_PART_HAVE_PARAM){
									allow=false;
									var part=this.get_part_instance(selectPart);
									//for(var key in part.params) alert(key);
									if(typeof(part.params)=='object' && part.params.length!=0) allow=true;
								}
								if(o.condition==IF_PART_OF_MODULE){
									var part=this.get_part_instance(this.seek_part());
									if(part){
										allow=(part.part_module!=0);
									}
								}
								if(o.condition==IF_CUR){
									if(!currentTable) allow=false;
									else {
										allow=true;
										o.table=currentTable;
										o.module=selectModule;
									}
								}
							}
							if(allow){
								// ADDITIONAL OPS
	
									// ADD / EDIT FORMS
									if(o.special==SP_ADD_COLS){
										var tbl_id=this.seek_table();
										var tbl=this.get_table_instance(tbl_id);
										this.rewrite_select_obj_for_select=tbl_id;
										this.rewrite_start_type_for_select=CMD_ROW;
										o.put=[];
										
										for(var key in tbl.cols){
											var ct=tbl.cols[key];
											if(ct.col_inform!=1) continue;	
											var to={
												'title': ct.col_name,
												'sname': ct.col_sname,
												'sname_quotes': 1,
												'type': CMD_MIXED
											};
											if(ct.col_type==0) to.type=CMD_STRING;
											if(ct.col_type==1 && ct.col_link2==0){
												to.type=STRING_ROW_SELECT;
												to.link=ct.col_link;
											}
											if(ct.col_type==1 && ct.col_link2==1){
												to.type=STRING_ROW_SELECT;
												to.is_array=1;
												to.link=ct.col_link;
											}
											if(ct.col_type==2){
												to.type=CMD_LOGICAL;
											}
											if(ct.col_type==3){
												to.type=STRING_FILENAME;
											}
											if(ct.col_type==4){
												if(ct.module_url) to.type=CMD_EX;
												else to.type=CMD_PART;
											}
											if(ct.col_type==5) to.type=CMD_USER;
											//if(ct.col_type==1) to.table=ct.col_link;
											if(ct.col_type==1 && ct.col_deep!=0){
												//to.table=ct.col_deep;
												//to.link=ct.col_deep; ??
											}
											
											o.put[o.put.length]=to;
										}
									}
	
								// INSERT BUTTON
								this.buttons[this.buttons.length]=new this.button(this,o,0,0);
							}
						}
					}
				}
			} else {
				if(this.prev && this.prev.selectedButton){
				
					var op=this.prev.selectedButton.op;
					
					// SELPART PARAMS
					if(op.special==SP_PART_PARAM){
						var part=this.get_part_instance(selectPart);
						for(var key in part.params){
							var p=part.params[key];
							var o={
								'title': p.param_name,
								'id': key,
								'cmd': '$'+p.param_sname,
								'result': CMD_MIXED
							}
							this.buttons[this.buttons.length]=new this.button(this,o,c,1);
						}
					}
					
					// WIDGETS
					if(op.special==SP_WIDGET){
						if(!op.step){
							for(var key in x_widget){
								var w=x_widget[key];
								var options=[];
								for(var ex in w.exs) options[ex]=w.exs[ex];
								var o={
									'title': w.module_name,
									'id': w.module_sname,
									'cmd':w.module_sname,
									'step':1,
									'disable_insert':true,
									'special': SP_WIDGET,
									'put': [{
										'title':'Экземпляр',
										'type': CMD_EX,
										'list': options
									}],
									'result':-1
								}
								this.buttons[this.buttons.length]=new this.button(this,o,c,1);
							}
						}
						if(op.step==1){
							var w=x_widget[op.id];
							for(var key in w.parts){
								var part=w.parts[key];
								var o={
									'title': part.part_name,
									'text': part.part_about,
									'id': part.part_sname,
									'cmd':part.part_sname,
									'part':part.part_id,
									'step':2,
									'put':[],
									'result': CMD_STRING//CMD_MIXED //CMD_PART
									//'special': SP_WIDGET
								}
								if(part['params']) for(var k in part['params']){
									var p=part['params'][k];
									var pp=o.put[o.put.length]={
										title: p['param_name'],
										sname: p['param_sname']
									}
									if(p['param_array']==1) pp.is_array=1;
									if(p['param_type']==4) pp.type=CMD_LOGICAL;
									if(p['param_type']==1 && p['param_list']){
										var l=p['param_list'].split("\r\n");
										pp.list=Array();
										for(var i=0;i<l.length;i++){
											var ll=l[i].split('=');
											pp.list["'"+ll[1]+"'"]=ll[0];
										}
									}
									//if(p['param_type']==2) pp.type //предлагать выбрать из таблицы param_link (вопрос как выбрать экземпляр)
									if(p['param_type']==2){
										pp.type=STRING_ROW_SELECT;
										pp.link=p.param_link;
									}
									if(p['param_type']==5) pp.type=CMD_ROW;
									if(p['param_type']==6) pp.type=CMD_ARRAY;
									if(p['param_type']==7) pp.type=STRING_COLNAME_PARAM;
									if(p['param_type']==8) pp.type=CMD_EX;
									if(p['param_type']==9){
										pp.type=STRING_GROUP_NAME;
										pp.in_quotes=1;
									}
									if(p['param_type']==10) pp.type=STRING_PART_NAME;
									if(p['param_default']) pp.default=p['param_default'];
								}
								this.buttons[this.buttons.length]=new this.button(this,o,c,2);
							}
						}
					}
					// COMPONENTS
					if(op.special==SP_COMPONENT){
						var c_type=0;
						if(op.cmd=='func' || op.cmd=='function' || op.cmd=='_func' || op.cmd=='_function') c_type=0;
						if(op.cmd=='show' || op.cmd=='_show') c_type=1;
						if(op.cmd=='component' || op.cmd=='_component') c_type=2;
						if(op.cmd=='form' || op.cmd=='_form') c_type=3;
						if(!op.step){
							if(x_component && x_component[c_type])
							for(var key in x_component[c_type]){
								var c=x_component[c_type][key];
								if(c.name){
									var o={
										'title': c.name,
										'id': key,
										'cmd':op.cmd,
										'step':1,
										'disable_insert':true,
										'skip_folder':true,
										'special':SP_COMPONENT,
										'result':-1
									}
									this.buttons[this.buttons.length]=new this.button(this,o,c,1);
								}
							}
						}
						if(op.step==1){						
							if(x_component[c_type][op.id]['parts'])
							for(var key in x_component[c_type][op.id]['parts']){
								var c=x_component[c_type][op.id]['parts'][key];
								if(c.part_name){
									var o={
										'title': c.part_name,
										'text': c.part_about,
										'id': key,
										'part':c.part_id,
										'cat_id':op.id,
										'step':2,
										/*'cmd':op.cmd,*/
										'cmd': c.part_sname,
										'result': CMD_MIXED,
										'put': []
									}
									if(c['params']) for(var k in c['params']){
										var p=c['params'][k];
										var pp=o.put[o.put.length]={
											title: p['param_name'],
											sname: p['param_sname']
										}
										if(p['param_array']==1) pp.is_array=1;
										if(p['param_type']==4) pp.type=CMD_LOGICAL;
										if(p['param_type']==1 && p['param_list']){
											var l=p['param_list'].split("\r\n");
											pp.list=Array();
											for(var i=0;i<l.length;i++){
												var ll=l[i].split('=');
												pp.list["'"+ll[1]+"'"]=ll[0];
											}
										}
										//if(p['param_type']==2) pp.type //предлагать выбрать из таблицы param_link (вопрос как выбрать экземпляр)
										if(p['param_type']==2){
											pp.type=STRING_ROW_SELECT;
											pp.link=p.param_link;
										}
										if(p['param_type']==5) pp.type=CMD_ROW;
										if(p['param_type']==6) pp.type=CMD_ARRAY;
										if(p['param_type']==7) pp.type=STRING_COLNAME_PARAM;
										if(p['param_type']==8) pp.type=CMD_EX;
										if(p['param_type']==9){
											pp.type=STRING_GROUP_NAME;
											pp.in_quotes=1;
										}
										if(p['param_type']==10) pp.type=STRING_PART_NAME;
										if(p['param_default']) pp.default=p['param_default'];
									}
									this.buttons[this.buttons.length]=new this.button(this,o,c,2);
								}
							}
						}
					}
					// IF EX PARAM
					if(op.special==SP_EX_PARAM){
						var m=this.get_module_instance(op.module);
						for(var key in m.params){
							var ct=m.params[key];
							var to={
								'title': ct.col_name,
								'cmd': ct.col_sname,
								'result': CMD_MIXED
							};
							if(ct.col_type==0) to.result=CMD_STRING;
							if(ct.col_type==1 && ct.col_link2==0) to.result=CMD_ROW;
							if(ct.col_type==1 && ct.col_link2==1){ to.result=CMD_ARRAY; to.result_long=CMD_ROW; }
							if(ct.col_type==2) to.result=CMD_STRING;
							if(ct.col_type==3){
								to.result=/*CMD_COL_FILE*/STRING_FILENAME;
								to.cmd_type=CMD_STRING;
							}
							//if(ct.col_type==4) to.result=CMD_EX;
							if(ct.col_type==4){
								if(ct.module_url) to.result=CMD_EX;//CMD_ROW_MODULE;//CMD_EX;
								else to.result=CMD_PART;
							}
							if(ct.col_type==5) to.result=CMD_USER;
							if(ct.col_type==6) to.result=CMD_STRING;
							//if(ct.col_type==7) to.result=CMD_GROUP;
							if(ct.col_type==1) to.table=ct.col_link;
							if(ct.col_type==1 && ct.col_deep!=0) to.table=ct.col_deep;
							this.buttons[this.buttons.length]=new this.button(this,to,ct,0);
						}
					}
					// IF PARTS
					this.get_buttons_from_parts=function(owner,parts,step){
						if(!(owner in parts)) return false;
						for(key in parts[owner]){
							c=parts[owner][key];
							var add_name='';
							if(step>0){
								add_name='- ';
								for(var i=0;i<step;i++) add_name='&nbsp; &nbsp;'+add_name;
							}
							var o={
								'title': add_name+c.part_name,
								'text': c.part_about,//(typeof(c.part_about)!='undefined'?c.part_about:'123'),
								'id': key,
								'part':c.part_id,
								'cmd':c.part_sname,
								'result': CMD_PART,
								'float':'left',
								'put': []
							}
							if(c['params']) for(var k in c['params']){
								var p=c['params'][k];
								var pp=o.put[o.put.length]={
									title: p['param_name'],
									sname: p['param_sname']
								}
								if(p['param_array']==1) pp.is_array=1;
								if(p['param_type']==4) pp.type=CMD_LOGICAL;
								if(p['param_type']==1 && p['param_list']){
									var l=p['param_list'].split("\r\n");
									pp.list=Array();
									for(var i=0;i<l.length;i++){
										var ll=l[i].split('=');
										pp.list["'"+ll[1]+"'"]=ll[0];
									}
								}
								//if(p['param_type']==2) pp.type //предлагать выбрать из таблицы param_link (вопрос как выбрать экземпляр)
								if(p['param_type']==2){
									pp.type=STRING_ROW_SELECT;
									pp.link=p.param_link;
								}
								if(p['param_type']==5) pp.type=CMD_ROW;
								if(p['param_type']==6) pp.type=CMD_ARRAY;
								if(p['param_type']==7) pp.type=STRING_COLNAME_PARAM;
								if(p['param_type']==8) pp.type=CMD_EX;
								if(p['param_type']==9){
									pp.type=STRING_GROUP_NAME;
									pp.in_quotes=1;
								}
								if(p['param_type']==10) pp.type=STRING_PART_NAME;
								if(p['param_default']) pp.default=p['param_default'];
							}
							this.buttons[this.buttons.length]=new this.button(this,o,c,2);
							this.get_buttons_from_parts(key,m.parts,step+1);
						}
					}
					if(op.special==SP_PART){
						this.from_part=1;
						/*if(op.module) */op.module=this.seek_module();
						var m=this.get_module_instance(op.module);
						if(m) this.get_buttons_from_parts(0,m.parts,0);
					}
				}
			}
			
			// ADDITIONAL BUTTONS BOTTOM
			if(this.prev && this.prev.selectedButton){
				var po=prev.selectedButton.op;
				if(po.from=='col' || (po.cmd=='col' && po.cmd_type==CMD_ROW)){
					var to={
						'title': 'Свойства поля',
						'cmd': 'col',
						'result': CMD_COL
					}
					this.buttons[this.buttons.length]=new this.button(this,to,0,0);
					
					var to={
						'title': 'Установить значение (с обработчиком)',
						'cmd': 'set_with_handler',
						'result': CMD_NONE,
						'put': [{
							'title': 'Новое значение',
							'type': CMD_MIXED,
							'req': 1
						}]
					}
					this.buttons[this.buttons.length]=new this.button(this,to,0,0);
					
					var to={
						'title': 'Установить значение',
						'cmd': 'set',
						'result': CMD_NONE,
						'put': [{
							'title': 'Новое значение',
							'type': CMD_MIXED,
							'req': 1
						}]
					}
					this.buttons[this.buttons.length]=new this.button(this,to,0,0);
				}
			}
			if(start_type==CMD_ARRAY){
					var to={
						'title': 'Выбрать значение по индексу',
						'cmd': '',
						'special': SP_ARRAY_INDEX,
						'result': CMD_MIXED,
						'put': [{
							'title': 'Индекс массива',
							'type': CMD_MIXED,
							'req': 1
						}]
					}
					this.buttons[this.buttons.length]=new this.button(this,to,0,0);
			}
			
			if(start_type in x_cmd.work_on){
				var to={
					'title': 'Остальные команды',
					'cmd': '',
					'special': SP_IGNORE,
					'disable_insert': 1,
					'result': this.prev.selectedButton.op.cmd_type
				}
				this.buttons[this.buttons.length]=new this.button(this,to,0,0);
			}
			
		}
		
		this.redraw_buttons(start_type);
		
		// SELECT object panel
		this.object_select=function(object, type){
			this.redraw_buttons(type,object);
		}
		this.have_object_select=0;
		this.have_type_select=0;
		this.from_part=(this.prev && this.prev.selectedButton.op.special==SP_PART);
		if(this.rewrite_start_type_for_select || this.inputObjectType || start_type==CMD_MIXED || start_type==CMD_MODULE || this.from_part){
			var s_object=0;
			var old_start_type=start_type;
			// DEFINE type
			if(start_type==CMD_MIXED){
				var tp=this.prev;
				while(tp){
					if(tp.selectedButton && tp.selectedButton.op.result_long){
						start_type=tp.selectedButton.op.result_long;
						break;
					}
					if(tp.selectedButton.op.table && tp.selectedButton.op.table){
						start_type=CMD_ROW;
						s_object=tp.selectedButton.op.table;
						break;
					}
					if(tp.selectedButton.op.module){
						start_type=CMD_EX;//or CMD_MODULE
						s_object=tp.selectedButton.op.module;
						break;
					}
					if(tp.selectedButton.op.col){
						start_type=CMD_COL;
						s_object=tp.selectedButton.op.col;
						break;
					}
					if(tp.selectedType && tp.selectedObject){
						start_type=tp.selectedType;
						s_object=tp.selectedObject;
						break;
					}
					tp=tp.prev;
				}
			}
			// DEFINE object
			if(start_type==CMD_ROW && !s_object){
				s_object=this.seek_table();
			}
			if((start_type==CMD_EX || start_type==CMD_MODULE || this.from_part) && !s_object){			
				s_object=this.seek_module();
			}
			if(this.rewrite_select_obj_for_select) s_object=this.rewrite_select_obj_for_select;
			
			var o='';
			this.have_object_select=1;
			// SELECT type
			var total_start_type=start_type;
			if(this.prev && this.prev.selectedButton.op.spec_result) total_start_type=this.prev.selectedButton.op.spec_result;
			//if(this.rewrite_start_type_for_select) start_type=this.rewrite_start_type_for_select;
			if(old_start_type==CMD_MIXED){
				o+='<div class="select_type_box">тип: <select class="select_type select_box" OnKeyDown="return select_refocus(event,1);" OnKeyPress="return select_refocus(event,2);" OnKeyUp="return select_refocus(event,3);">';
				o+='<option value="0">выберите тип</option>';
				o+='<option value="'+CMD_ROW+'"'+(total_start_type==CMD_ROW?' selected':'')+'>объект</option>';
				o+='<option value="'+CMD_STRING+'"'+(total_start_type==CMD_STRING?' selected':'')+'>текст</option>';
				o+='<option value="'+CMD_ARRAY+'"'+(total_start_type==CMD_ARRAY?' selected':'')+'>массив</option>';
				o+='<option value="'+CMD_MODULE+'"'+(total_start_type==CMD_MODULE?' selected':'')+'>модуль</option>';
				o+='<option value="'+CMD_EX+'"'+(total_start_type==CMD_EX?' selected':'')+'>экземпляр</option>';
				o+='<option value="'+CMD_TABLE+'"'+(total_start_type==CMD_TABLE?' selected':'')+'>таблица</option>';
				o+='<option value="'+CMD_COL+'"'+(total_start_type==CMD_COL?' selected':'')+'>переменная таблицы</option>';
				o+='<option value="'+CMD_USER+'"'+(total_start_type==CMD_USER?' selected':'')+'>пользователь</option>';
				o+='<option value="'+CMD_GROUP+'"'+(total_start_type==CMD_GROUP?' selected':'')+'>группа пользователей</option>';
				o+='<option value="'+CMD_PART+'"'+(total_start_type==CMD_PART?' selected':'')+'>часть</option>';
				o+='<option value="'+CMD_ZONE+'"'+(total_start_type==CMD_ZONE?' selected':'')+'>зона</option>';
				o+='</select>';
				o+='</div>';
				this.have_type_select=1;
			}
			// SELECT table
			o+='<div class="type_table type_box select_'+CMD_ROW+'_box" style="display: none;">таблица: <select class="select_table select_box" OnKeyDown="return select_refocus(event,1);" OnKeyPress="return select_refocus(event,2);" OnKeyUp="return select_refocus(event,3);">';
			o+='<option value="0">выберите таблицу</option>';
			if(x_module){
				for(var key in x_module.tables){
					var t=x_module.tables[key];
					o+='<option value="'+t['table_id']+'"'+((start_type==CMD_ROW  || this.rewrite_start_type_for_select==CMD_ROW) && t['table_id']==s_object?' selected':'')+'>'+t['table_name']+'</option>';
				}
				for(var fm in x_module.foreign) for(var key in x_module.foreign[fm].tables){
					var t=x_module.foreign[fm].tables[key];
					o+='<option value="'+t['table_id']+'"'+((start_type==CMD_ROW  || this.rewrite_start_type_for_select==CMD_ROW) && t['table_id']==s_object?' selected':'')+'>'+t['table_name']+' ('+x_module.foreign[fm].name+')'+'</option>';
				}
			}
			o+='</select></div>';
			// SELECT ex
			/*o+='<div class="type_ex type_box select_'+CMD_EX+'_box" style="display: none;">экземпляр: <select class="select_ex select_box">';
			o+='<option value="0">выберите экземпляр</option>';
			if(x_module){
				for(var key in x_module.exs){
					var t=x_module.exs[key];
					o+='<option value="'+t['ex_id']+'"'+(start_type==CMD_EX && t['ex_id']==s_object?' selected':'')+'>'+t['ex_name']+'</option>';
				}
			}
			o+='</select></div>';*/
			// SELECT module
			o+='<div class="type_module type_box select_'+CMD_MODULE+'_box" style="display: none;">модуль: <select class="select_module select_box" OnKeyDown="return select_refocus(event,1);" OnKeyPress="return select_refocus(event,2);" OnKeyUp="return select_refocus(event,3);">';
			o+='<option value="0">выберите модуль</option>';
			if(x_module){
				o+='<option value="'+x_module.id+'"'+((start_type==CMD_MODULE || start_type==CMD_EX || this.from_part || this.rewrite_start_type_for_select==CMD_MODULE || this.rewrite_start_type_for_select==CMD_EX) && x_module.id==s_object?' selected':'')+'>'+x_module.name+'</option>';
				for(var key in x_module.foreign){
					var t=x_module.foreign[key];
					o+='<option value="'+t['id']+'"'+((start_type==CMD_MODULE || start_type==CMD_EX || this.from_part || this.rewrite_start_type_for_select==CMD_MODULE || this.rewrite_start_type_for_select==CMD_EX) && t['id']==s_object?' selected':'')+'>'+t['name']+'</option>';
				}
			}
			o+='</select></div>';
			// DRAW boxes
			this.box.prepend('<div class="m-type-box">'+o+'</div>');
			if(total_start_type==CMD_ROW || this.rewrite_start_type_for_select==CMD_ROW){
				this.box.find('.type_table').show();
			}
			if(total_start_type==CMD_MODULE || total_start_type==CMD_EX || this.from_part || this.rewrite_start_type_for_select==CMD_MODULE || this.rewrite_start_type_for_select==CMD_EX){
				this.box.find('.type_module').show();
			}
			this.type_box=this.box.find('.m-type-box');
			var tf=this.object_select.bind(this);
			var tb=this.type_box;
			var o_this=this;
			this.type_box.find('.select_box').each(function(i,e){
				e.onchange=function(){
					var st=start_type;
					if(start_type==CMD_MIXED || old_start_type==CMD_MIXED){
						st=tb.find('.select_type').val();
					} else st=start_type;
					var val=this.options[this.selectedIndex].value;
					if(val!=0){
						tf(val,st);
					}
				}
			});
			if(start_type==CMD_MIXED || old_start_type==CMD_MIXED) this.type_box.find('.select_type').get(0).onchange=function(){
				var val=this.options[this.selectedIndex].value;
				if(val==0) return false;
				var ob=0;
				var osb=0;
				tb.find('.type_box').each(function(i,e){$(e).hide();});
				if(osb=tb.find('.select_'+val+'_box')){
					osb.show();
					var ob=osb.find('.select_box').val();
					//if(ob==0) return false;
				}
				tf(ob,val);
			}
			if(s_object!=0 || old_start_type!=total_start_type) this.redraw_buttons(total_start_type, s_object);
		}		
		
		// EVENT shadow folder scroller
		this.body_mousewheel=document.body.onmousewheel;
		this.mousewheel=function(e){/*selectButton=0;*/ if($(e.target).hasClass('m-shadow')) this.box.get(0).scrollTop=this.box.get(0).scrollTop+e.deltaY;}
		document.body.onmousewheel=this.mousewheel.bind(this);
		
		// FOLDER destructor
		this.destroy=function(){
			if(this.next) this.next.destroy();
			document.body.onmousewheel=this.body_mousewheel;
			this.box.remove();
			if(this.bar) this.bar.remove();
			//selectFolder=this.prev;
			selectFolder=this.old_selectFolder;
			selectButton=this.old_selectButton;
			selectOption=this.old_selectOption;
			pushedButton=this.old_pushedButton;
			//this.parent.folders[this.parent_id]=null;
			this.parent.folders.splice(this.parent.folders.length-1,1);
			folder_id--;
			if(!selectFolder) pushedButton=0;
		}
		
		// SELECT folder and navigation
		this.prev=selectFolder;
		selectFolder=this;
		selectButton=0;
	}
	
	// ADD first folder
	this.folders=[new this.folder(this,start_type)];
	
	// MANAGER destructor
	this.destroy=function(){
		for(var f in this.folders) if(this.folders[f]) this.folders[f].destroy();
		this.shadow.remove();
		this.box.remove();
		document.body.style.overflow = this.document_style_overflow;
		//selectManager=this.prev;

		selectManager=this.old_selectManager;
		selectFolder=this.old_selectFolder;
		selectButton=this.old_selectButton;
		selectOption=this.old_selectOption;
		pushedButton=this.old_pushedButton;
		
		manager[this.id]=null;
		im_id--;
	}
	
	//CLOSE button
	this.box.find('.m-top-red').get(0).onclick=this.destroy.bind(this);
	
	// SELECT manager
	this.prev=selectManager;
	selectManager=this;

}