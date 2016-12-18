<?php

// базовые операторы
include_once('operations.inc');

// типы результатов операторов

define('CMD_UNKNOWN',-1);
define('CMD_BASE',0);//команды корня
define('CMD_NONE',1);//нет результата
define('CMD_MIXED',2);//смешанный результат, определять по содержимому DATA
define('CMD_STRING',3);//имеется ввиду любое value отличное от других типов
define('CMD_ARRAY',4);
define('CMD_MODULE',5);
define('CMD_EX',6);
define('CMD_TABLE',7);
define('CMD_COL',8);
define('CMD_ROW',9);
define('CMD_USER',10);
define('CMD_GROUP',11);
define('CMD_PART',12);
define('CMD_STAT',13); //набор функций для работы с статистикой
define('CMD_CURTYPE',14); //результат, опирающийся на CUR_TYPE
define('CMD_ZONE',15);
define('CMD_FILE',16); //набор функций для работы с файловой системой
define('CMD_GLOBAL',17); //набор функция для работы с глобальными значениями
define('CMD_COL_FILE',18);//для работы с файлами-столбцами
define('CMD_COMPONENT',19);//php-компонент, размещённый на физическом сервере
//define('CMD_ROW_MODULE',20);//для работы с модульным значением строки

// Дополнительные типы для переменных операторов (для визуального редактора)
define('CMD_LOGICAL',20); //0, 1, true, false //для checkbox
define('CMD_IF',21); //набор условий //сразу активируется секция "логическое условие" в редакторе значений
define('CMD_FASTGET_IF',22);//набор условий для fastget-функции
define('CMD_FASTGET_ORDER',23);//набор параметров для сортировки fastget-функции
define('CMD_CMD',24); //команда, например cur.title.explode
define('CMD_STATIC',25);//статичное значение, определённое в поле visual->put[x]->static, при генерации указывается без скобочек (работает как чекбокс, если он отмечен - значение указывается, не может быть req)
define('CMD_DOUBLE',26);//вложенная группа из двух параметров, разделённых знаком :
define('CMD_USER_GROUP',27);//пользователь, группа или SNAME группы
define('CMD_COL_TYPE',28);//тип, соответствующий типу текущего поля
define('CMD_TPL',29);//шаблон

define('CMD_FIND',701);//fastget-find
define('CMD_FIND_COUNT',702);//fastget-find-count
define('CMD_FIND_WHERE',703);//fastget-find-where

// Мнимые подтипы для CMD_STRING
define('STRING_COLNAMES',101); //массив sname полей, написанных в строчку, заключённых в одинарные или двойные кавычки (возможно не используется)
define('STRING_COLNAME',102); //sname поля
define('STRING_COLNAMES_SEARCH',103);//массив sname полей, написанных в строчку, заключённых в одинарные или двойные кавычки, которые используются для поиска
define('STRING_COLNAMES_ORDER',104); //массив sname полей, написанных в строчку, заключённых в одинарные или двойные кавычки, которые используются для сортировки
define('STRING_SUBTABLE_NAMES',105); //sname подтаблицы
define('STRING_PARAM_NAME',106);//название переменной (пытается найти его среди текущей части, либо предложить пользователю свой вариант)
define('STRING_EX_PARAM',107);//sname одного из параметров модуля
define('STRING_TABLE_NAME',108);//sname одной из таблиц текущего модуля
define('STRING_GROUP_NAME',109);//sname группы
define('STRING_PART_NAME',110);//sname части
define('STRING_FUNCTION_NAME',111);//функция
define('STRING_SHOW_NAME',112);//отображение
define('STRING_COMPONENT_NAME',113);//компонент
define('STRING_FORM_NAME',114);//форма
define('STRING_TABLE_NAME_ID',115);//sname таблицы или её id (или сам объект таблица)
define('STRING_FILENAME',116);//путь до конкретного файла (или строка, генерирующая его) - здесь открывается файловый редактор
define('STRING_MODULE_TABLE',117);// module_sname.table_sname (для foreign_tables)
define('STRING_DATE',118);
define('STRING_TIME',119);
define('STRING_DATETIME',120);
define('STRING_NUM',121);//число
define('STRING_URL',122);//URL
define('STRING_BLOCK_NAME',123);//Block name
define('STRING_MODULE_SNAME',124); //sname модуля
define('STRING_COLNAME_ANY',125); // выбор colname из всех доступных col
define('STRING_COLNAME_PARAM',126); // выбор colname для part_param
define('STRING_ROW_SELECT',127);//определяет rows с помощью ajax запросов (2 select - экземпляр и rows, с учётом контроля доступа)
define('STRING_COLNAME_FIND',128); // выбор colname с поддержкой значений FASTGET

// Специальные типы операторов
define('SP_NONE',0);//обычные правила (указываются в массиве visual->put)
define('SP_COMPONENT',1);//правила для компонентов
define('SP_PART',2); //правила для части (тоже, что и для компонентов, но с привязкой к модулю - не уверен надо ли это)
				//отличие от SP_COMPONENT также в том, что вместо привычного part.part1(x=1), может быть part.part1.sname (т.е. отсылка на обработчик части)
define('SP_PREPARE',3); //устанавливает параметры обработки аргументов метода как для метода prepare (динамический набор столбцов)
define('SP_FASTGET',4); // сложный разбор, следующий op может быть subtable_sname
//define('SP_SC',5);// разбор для команды SC, SC2 (следующий op = col_sname)
//define('SP_ST',6);//разбор для команды ST (следующий op = table_sname)
define('SP_POINT',5);//разбор, при котором следующий за . элемент попадает в put[0] (без кавычек)
define('SP_SPACE',6);//разбор для команд первого уровня, когда в случае next_operation==' ', остаток помещается в PUT
define('SP_PUP',7);//разбор команд для PUP (с учётом его множественного повторения)
define('SP_UP',8);//разбор команд для UP (с учётом его множественного повторения)
define('SP_AJAX',9);//разбор команд для Ajax (далее переходит либо на SP COMPONENT, либо на SP PART
define('SP_UNKNOWN',10);//разбор неизвестного
define('SP_DOUBLE_POINT',11);//тоже, что SP_POINT, но используются две точки, т.е. не "op.x", а "op.x.y" (пример с foreign_tables)
define('SP_IGNORE',12);//игнорировать эту команду при компиляции
define('SP_EX_PARAM',13);//генерирование параметров экземпляра
define('SP_ARRAY_INDEX',14);//индекс массива (имитация [] после предыдущего оператора)
define('SP_WIDGET',15);//виджет
define('SP_CACHE',16);//кеш
define('SP_PART_PARAM',17);//параметры текущей части
define('SP_ADD_COLS',18);//все столбцы таблицы (для добавления или изменения её элемента)

// Условия для употребления команд
define('IF_CRON',1);//отображается, если работа ведётся с частью, вызванной по таймеру
define('IF_IMPORT',2);//может ли часть работать с импортом
define('IF_PART_CASE',3);//работает только в условии для вызова части
define('IF_TREE_NUMERIC',4);//работает только внутри числовых переборов (переборов от min до max)
define('IF_TABLE_BOTTOM',5);//только для обработчика подвала таблицы
define('IF_COW',6);//если определён COW (работа ведётся в части с определнием URL)
define('IF_CUR',7);//если определён CUR (т.е. действие происходит в теле цикла)
define('IF_MODULE',8);//если часть работает в модуле
define('IF_CUCOL',9);//если определён CUCOL (работа ведётся в обработчике поля таблицы)
define('IF_COL_FILE',10);//только если текущее поле - файл
define('IF_TABLE_TOP',11);
define('IF_TABLE_EDIT',12);
define('IF_COL_EDIT',13);
define('IF_COL_FORM',14);
define('IF_COL_SHOW',15);
define('IF_PART_DETECT',16);
define('IF_CUR_USER',17);
define('IF_WIDGET',18);
define('IF_PART_HAVE_PARAM',19);
define('IF_PART_OF_MODULE',20);

// Типы входного объекта
define('OB_NONE',0);
define('OB_TABLE',1); // таблица
define('OB_MODULE',2); // модуль

// операторы
//$timer=start_timer();
//$memory=memory_get_usage();
$cmd=Array();
$cmd_group=Array();
include_once('cmd/base.inc');
include_once('cmd/glob.inc');
include_once('cmd/col.inc');
include_once('cmd/zone.inc');
include_once('cmd/part.inc');
include_once('cmd/ex.inc');
include_once('cmd/module.inc');
include_once('cmd/table.inc');
include_once('cmd/row.inc');
include_once('cmd/arr.inc');
include_once('cmd/str.inc');
include_once('cmd/stat.inc');
include_once('cmd/file.inc');
include_once('cmd/user.inc');
include_once('cmd/group.inc');
//echo end_timer($timer).'<br>';
//$xnm=memory_get_usage();
//echo $xnm-$memory;

//$conter=0;
//foreach($cmd AS $name=>$types) foreach($types AS $type=>$command) $counter++;
//echo $counter;

// находит самый глубокий первый оператор (нужно для определения начального мутатора, например [limit ^rows.rows2*2+arows 10:9], из rows.rows2*2+arows поможет выбрать rows
function first_deep_op($ops){
	if(is_object($ops) && isset($ops->sub)) $ops=$ops->sub;//?
	if(empty($ops)) return false;
	if(is_array($ops)) $o=array_shiftA($ops); else $o=$ops;
	if($o->type==LEXER_SUB && !empty($o->sub)) return first_deep_op($o->sub); else return $o;
}

// находит самый глубокий первый оператор, и устанавливает в него мутатор $mutator
function set_deep_mutator(&$op,$mutator){
	if($op->type==LEXER_SUB && !empty($op->sub)) for($key=key($op->sub);$key<count($op->sub);$key++){//foreach($op->sub AS $key=>$value){
		set_deep_mutator($op->sub[$key],$mutator);
		return false;
	} else $op->mutator=$mutator;
}

// создание операции
function create_op($data,$type=LEXER_CMD,$separator='',$sub=Array(),$put=Array(),$arr=Array()){
	$r=new lexer_op();
	$r->data=$data;
	$r->type=$type;
	$r->next_operation=$separator;
	if(!empty($sub)) $r->sub=$sub;
	if(!empty($put)) $r->put=$put;
	if(!empty($arr)) $r->arr=$arr;
	return $r;
}

function add_new_op(&$ops,$op,$type,$move_forward=false){
	array_unshift($ops,$op);
	if($move_forward) next($ops);
}

// выдаёт первый разделитель из массива операторов
function cmd_separator(&$ops){
	//foreach($ops AS $op) return $op->next_operation;
	$c=current($ops);
	return ($c->next_operation);
	//return '';
}

// оставить ID от объекта, если переменная - объект
function parse_id(&$var){
	if(is_object($var) && isset($var->id) && !isset($var->rows)) $var=$var->id;
}

// находит в массиве ops ту, чья data==$data и удаляет его из массива
function shift_op(&$ops,$data){
	if(!empty($ops)) /*foreach($ops AS $key=>$op)*/ for($key=key($ops);$key<count($ops);$key++){
		if(!isset($ops[$key])) continue;
		$op=$ops[$key];
		if(isset($op->data) && $op->type==LEXER_CMD && $op->data==$data){
			$k=key($ops);
			array_splice($ops,$key,1);
			seek_key($ops,$k,1);
			return $op;
		}
	}
	return false;
}

// выдаёт data первого op и удаляет его
function get_first_data(&$ops){
	$x=array_shiftA($ops);
	if(isset($x->data)) return $x->data; else return '';
}

// выдаёт первый op и удаляет его
function get_first_op(&$ops){
	return array_shiftA($ops);
}

// выдаёт значение или data оператора
function get_real_data(&$op){
	if(!is_object($op)) return false;
	if($op->type==LEXER_CMD && empty($op->put) && empty($op->sub)) return $op->data;
	else return parse_var($op);
}

// выдаёт data первого op и НЕ удаляет его
function get_first_data_safe(&$ops){
	if(!empty($ops)){
		$key=key($ops);
		return $ops[$key]->data;
	} else return false;
}

// если первый op в массиве имеет data==data, то он возвращается функцией и удаляется из массива $ops;
function shift_first_op(&$ops,$data){
	//if(!empty($ops)) /*foreach($ops AS $key=>$op)*/ for($key=key($ops);$key<count($ops);$key++){
	//	$op=$ops[$key];
	//	if(isset($op->data) && $op->type==LEXER_CMD && $op->data==$data){
	//		//unset($ops[$key]);
	//		$k=key($ops);
	//		array_splice($ops,$key,1);
	//		seek_key($ops,$k,1);
	//		return $op;
	//	} else return false;
	//
	if(!empty($ops)){
		$key=key($ops);
		$cop=&$ops[$key];
		if(isset($cop->data) && $cop->type==LEXER_CMD && $cop->data==$data){
			return array_shiftA($ops);
		}
	}
	return false;
}

// превращение ID в объект (при необходимости, возможно следует отключить)
function prepend_type($id,$type='row',$add=Array()){
	if(!is_object($id)){
		$res->id=$id;
		$res->type=$type;
	} else {
		$res=$id;
		$res->type=$type;
	}
	if(isset($add['module'])) $res->module=$add['module'];
	if(isset($add['ex'])) $res->ex=$add['ex'];
	if(isset($add['tex'])) $res->tex=$add['tex'];
	if(isset($add['table'])) $res->table=$add['table'];
	if(isset($add['col'])) $res->col=$add['col'];
	if(isset($add['row'])) $res->row=$add['row'];
	if(isset($add['own'])) $res->own=$add['own'];
	if(isset($add['owner'])) $res->own=$add['owner'];
	if(isset($add['name'])) $res->name=$add['name'];
	if(isset($add['sname'])) $res->sname=$add['sname'];
	return $res;
}

function search_operations(&$ops,$operations=Array(),$include_arr=false,$include_put=false){
	//foreach($ops AS $index=>$op){
	$k=key($ops);
	$c=count($ops);
	//while(!((list($index,$op)=each($ops))===false)){
	for($index=$k;$index<$c;$index++){
		$x=&$ops[$index];
		if(!empty($x->next_operation) && isset($operations[$x->next_operation])){
			//seek_key($ops,$k);
			return $x;
		}
		if($x->type==LEXER_SUB && isset($x->sub)){
			$z=search_operations($x->sub,$operations,$include_arr,$include_put);
			if($z){
				//seek_key($ops,$k);
				return $z;
			}
		}
		if(isset($x->put) && $include_put){
			$z=search_operations($x->put,$operations,$include_arr,$include_put);
			if($z){
				//seek_key($ops,$k);
				return $z;
			}
		}
		if(isset($x->arr) && $include_arr){
			foreach($x->arr AS $index_arr=>$arr){
				$y=&$x->arr[$index_arr];
				$z=search_operations($y,$operations,$include_arr,$include_put);
				if($z){
					//seek_key($ops,$k);
					return $z;
				}
			}
		}
	}
	//seek_key($ops,$k);
	return false;
}

//проверяет, есть ли в связях между OPS что-то из Operations
function check_single(&$ops,$operations,$as_sub=false){
	$operations=array_flip($operations);
	if(is_object($ops)) $ops=Array($ops);
	if(count($ops)<=1) return false;
	if(empty($ops)) return false;
	$cops=count($ops);
	for($index=key($ops);$index<$cops;$index++){
	//foreach($ops AS $index=>$op){
		if(!empty($ops[$index]->next_operation) && isset($operations[$ops[$index]->next_operation])) return false;//хороший исход - нужный оператор найден
	}
	if(!$as_sub){
		//standart
		$ops=Array($ops);
	} else {
		//for fastget filters only
		$nop=new lexer_op();
		$nop->type=LEXER_SUB;
		$nop->sub=$ops;
		$ops=Array($nop);
	}
}

// не используется
function load_cmd_file($cmd_type){
	global $cmd;
	if($cmd_type==CMD_BASE) 								include_once('cmd/base.inc');
	if($cmd_type==CMD_GLOBAL) 							include_once('cmd/glob.inc');
	if($cmd_type==CMD_COL || $cmd_type==CMD_COL_FILE)	include_once('cmd/col.inc');
	if($cmd_type==CMD_ZONE)								include_once('cmd/zone.inc');
	if($cmd_type==CMD_PART)								include_once('cmd/part.inc');
	if($cmd_type==CMD_EX) 								include_once('cmd/ex.inc');
	if($cmd_type==CMD_MODULE) 							include_once('cmd/module.inc');
	if($cmd_type==CMD_TABLE) 							include_once('cmd/table.inc');
	if($cmd_type==CMD_ROW) 								include_once('cmd/row.inc');
	if($cmd_type==CMD_ARRAY) 							include_once('cmd/arr.inc');
	if($cmd_type==CMD_STRING) 							include_once('cmd/str.inc');
	if($cmd_type==CMD_STAT) 								include_once('cmd/stat.inc');
	if($cmd_type==CMD_FILE) 								include_once('cmd/file.inc');
	if($cmd_type==CMD_USER) 								include_once('cmd/user.inc');
	if($cmd_type==CMD_GROUP) 							include_once('cmd/group.inc');
}

$cmd['blank'][CMD_BASE]->process=function(&$data,&$op,&$ops){
	return false;
};

function get_cmd_link($name,$cmd_type){
	global $cmd;
	//load_cmd_file($cmd_type);
	if(isset($cmd[$name][$cmd_type])) return $cmd[$name][$cmd_type]->process;
	else return $cmd['blank'][CMD_BASE]->process;
}

// нахождение подходящего, либо динамического CMD
function find_cmd($cmd_name,$cmd_type=CMD_BASE,&$data='',$owner=0){
	global $cmd;
	stf('find_cmd');
	if($cmd_type==CMD_MIXED || ($cmd_type==CMD_BASE && !empty($owner))){
		if(is_object($data)){
			if(isset($data->rid)) $cmd_type=CMD_ROW;
			else if(isset($data->rows) && is_array($data->rows)) $cmd_type=CMD_ARRAY;
			else if(isset($data->type)){
				if($data->type=='row') $cmd_type=CMD_ROW;
				else if($data->type=='cl' || $data->type=='col') $cmd_type=CMD_COL;
				else if($data->type=='tbl' || $data->type=='table') $cmd_type=CMD_TABLE;
				else if($data->type=='ex') $cmd_type=CMD_EX;
				else if($data->type=='md' || $data->type=='module') $cmd_type=CMD_MODULE;
				else if($data->type=='usr' || $data->type=='user') $cmd_type=CMD_USER;
				else if($data->type=='grp' || $data->type=='group') $cmd_type=CMD_GROUP;
				else if($data->type=='zn' || $data->type=='zone') $cmd_type=CMD_ZONE;
				else if($data->type=='prt' || $data->type=='part') $cmd_type=CMD_PART;
				else if($data->type=='component') $cmd_type=CMD_COMPONENT;
				else $cmd_type=CMD_STRING;
			} else $cmd_type=CMD_STRING;
		} else if(is_array($data)) $cmd_type=CMD_ARRAY;
		else $cmd_type=CMD_STRING;
		
		if(is_object($data) && isset($data->rows) && is_array($data->rows)) $cmd_type=CMD_ARRAY;
		else if(is_object($data) && isset($data->type) && $data->type=='row') $cmd_type=CMD_ROW;
		else if(is_object($data) && isset($data->rid)) $cmd_type=CMD_ROW;
		else if(is_object($data) && isset($data->type) && $data->type=='cl') $cmd_type=CMD_COL;
		else if(is_object($data) && isset($data->type) && $data->type=='tbl') $cmd_type=CMD_TABLE;
		else if(is_object($data) && isset($data->type) && $data->type=='ex') $cmd_type=CMD_EX;
		else if(is_object($data) && isset($data->type) && $data->type=='md') $cmd_type=CMD_MODULE;
		else if(is_object($data) && isset($data->type) && $data->type=='usr') $cmd_type=CMD_USER;
		else if(is_object($data) && isset($data->type) && $data->type=='grp') $cmd_type=CMD_GROUP;
		else if(is_object($data) && isset($data->type) && $data->type=='zn') $cmd_type=CMD_ZONE;
		else if(is_object($data) && isset($data->type) && $data->type=='prt') $cmd_type=CMD_PART;
		else if($cmd_type==CMD_MIXED){
			if(is_array($data)) $cmd_type=CMD_ARRAY;
			else $cmd_type=CMD_STRING;
		}
	}
	if($cmd_type==CMD_CURTYPE){
		global $cur_type;
		if(empty($cur_type)) $cur_type='row';
		if($cur_type=='row') $cmd_type=CMD_ROW;
		else if($cur_type=='cl') $cmd_type=CMD_COL;
		else if($cur_type=='tbl') $cmd_type=CMD_TABLE;
		else if($cur_type=='ex') $cmd_type=CMD_EX;
		else if($cur_type=='md') $cmd_type=CMD_MODULE;
		else if($cur_type=='usr') $cmd_type=CMD_USER;
		else if($cur_type=='grp') $cmd_type=CMD_GROUP;
		else if($cur_type=='zn') $cmd_type=CMD_ZONE;
		else if($cur_type=='prt') $cmd_type=CMD_PART;
		else $cmd_type=CMD_STRING;
	}
	//load_cmd_file($cmd_type);
	//if(!isset($cmd[$cmd_name][$cmd_type]) && !isset($cmd[''][$cmd_type])){
	//	echo $cmd_name.' - '.$cmd_type.'<br>';
	//}
	if(isset($cmd[$cmd_name]) && isset($cmd[$cmd_name][$cmd_type])){
		$r=new stdClass();
		$r->status=1;
		$r->c=&$cmd[$cmd_name][$cmd_type];
		while(isset($r->c->alias)){
			if(isset($r->c->result) && !isset($r->type)) $r->type=$r->c->result;
			$r->c=&$r->c->alias;
		}
		if(!isset($r->type)) $r->type=$r->c->result;//$cmd_type;
		$r->n=$cmd_name;
		$r->t=$cmd_type;
		etf('find_cmd');
		return $r;
	} else {
		// Поиск не названных операций
		if(isset($cmd[''][$cmd_type])){
			$r->c=&$cmd[''][$cmd_type];
			while(isset($r->c->alias)) $r->c=&$r->c->alias;
			$r->status=1;
			$r->type=$r->c->result;
			$r->n='';
			$r->n2=$cmd_name;
			$r->t=$cmd_type;
			etf('find_cmd');
			return $r;
		} else {
			// Если ничего не найдено - выдавать пустой результат
			$r->status=0;
			$r->type=CMD_UNKNOWN;
			etf('find_cmd');
			return $r;
		}
	}
}

// выполнение команды
function do_cmd(&$op,&$ops,&$cmd,&$data){
	//stf('do_cmd');
	if($cmd->status==1){
		$p=&$cmd->c->process;
		$n=$cmd->n;
		if(isset($cmd->n2)) $n=$cmd->n2;
		stf('cmd_'.$n);
		$r=$p($data,$op,$ops);//после теста вернуть return $p($data,$op,$ops);		
		etf('cmd_'.$n);
		//etf('do_cmd');
		return $r;
	} else {
		//etf('do_cmd');
		return $data;
	}
}

// Проверка на равенство
function seek_equal($x,$y){
	parse_id($x); parse_id($y);
	if(empty($x) && empty($y)){
		return true;
	} else if((is_string($x) || is_string($y)) && !is_object($x) && !is_array($x) && !is_object($y) && !is_array($y)){
		return ((string)$x==(string)$y);
	} else {
		return ($x==$y);
	}
}

// обход вставок кода
function walk_embeded(&$ops){
	$cops=count($ops);
	for($key=key($ops);$key<$cops;$key++){
		if($ops[$key]->type==LEXER_SUB && !empty($ops[$key]->sub)) walk_embeded($ops[$key]->sub);
		if(isset($ops[$key]->embed)){
			$move=0;
			//if(!empty($ops[$key]->old_data))/* <- ?? */ $ops[$key]->data=$ops[$key]->old_data;
			foreach($ops[$key]->embed AS $e){
				$res='';
				if($e->type==EMBED_PARSE){
					$res=parse_var($e->data);
				} else if($e->type==EMBED_IMPRINT){
					//$res='"'.$e->data.'"';
					$res='"'.str_replace('"','\\"',str_replace('\\','\\\\',$e->data)).'"';
				} else {
					$res=shell_tpl($e->data);
					if($e->type==EMBED_STRING){
						$res='"'.str_replace('"','\\"',str_replace('\\','\\\\',$res)).'"';
						$vv=1;
					}
				}
				$left_margin=0;
				if($e->type!=EMBED_TPL) $left_margin=1;
				if(!empty($res) || $res==0){
					$ops[$key]=clone $ops[$key];
					if(!isset($ops[$key]->data)) $ops[$key]->data='';
					if(is_object($res) && isset($res->id)) $res=$res->id;
					if(is_array($res)) $res='';
					//echo '<br>pre: '.$ops[$key]->data.'<br>';
					//echo '<br>start: '.$e->start.'<br>';
					$ops[$key]->data=substr($ops[$key]->data,0,$e->start+$move).$res.substr($ops[$key]->data,$e->start+$move+$left_margin);
					//echo '<br>post: '.$ops[$key]->data.'<br>';
					$move+=strlen($res);
				}
			}
			$tmp=shell_lexer($ops[$key]->data);
			if($ops[$key]->next_operation!='.' && ($key==0 || $ops[$key-1]->next_operation!='.')){
				$tmp=Array(create_op('',LEXER_SUB,$ops[$key]->next_operation,$tmp));
			}
			$ctmp=count($tmp)-1;
			$tmp[$ctmp]->next_operation=$ops[$key]->next_operation;
			if(!empty($ops[$key]->put) && empty($tmp[$ctmp])) $tmp[$ctmp]->put=$ops[$key]->put;
			if(count($tmp)==1 && !empty($tmp[0])) $ops[$key]=$tmp[0];
			if(count($tmp)>1){
				$k=key($ops);
				array_splice($ops,$key,1,$tmp);
				seek_key($ops,$k);
			}
			unset($ops[$key]->embed);//new
			if(isset($ops[$key]->embeded)) $ops[$key]->embeded=false;
		}
	}
	if(isset($ops[0]->embeded)) $ops[0]->embeded=false;
}

// основная функций разбора цепочек команд
function parse_var(&$hops/*$ops*/,$owner=0,$owner_data='',$owner_cmd_type=CMD_BASE){
	/*$owner=0;
	$owner_data='';
	$owner_cmd_type=CMD_BASE;*/
	
	//var_dump($hops);
	
	if(empty($hops)) return false;
	if(!is_array($hops) && !is_object($hops)){
		$hops=shell_lexer($hops);
	}
	
	//if(!is_array($hops)) $hops=Array($hops);
	if(!is_array($hops)){
		$ops=Array(clone $hops);
	} else {
		$ops=Array();
		foreach($hops AS $value) $ops[]=clone $value;
	}
	
	//$k=key($hops);
	//foreach($hops AS $var=>&$value) $ops[$var]=clone $value;	
	//array_walk($hops, 'array_clone',&$ops);
	//seek_key($ops,$k,1);

	//$k=key($hops);	
	/*$tmp = new ArrayObject($hops);
	$ops=$tmp->getArrayCopy();//$hops;*/
	/*if(!empty($hops)){
		$tmp = new ArrayIterator($hops);
		$ops=$tmp->getArrayCopy();
	} else $ops=$hops;*/
	//$ops=(array)clone(object)$hops;
	//$ops=unserialize(serialize($hops));
	//seek_key($ops,$k,1);

	global $db, $operations_a;
	
	/*if(empty($ops)) return false;
	
	if(!is_array($ops) && !is_object($ops)){
		$ops=shell_lexer($ops);
	}*/
	
	//stf('parse_var');
	
	//вместо этого нужно проработать array_shiftA так, чтобы он занулял указатель только для текущего прогона parse_ver...
	//if(empty($owner)) $ops=dc($ops); //если будут проблемы с тормозми - закомментить эту строчку, одна это может привести к опасным ситуациям
	//if(empty($owner)) $ops=unserialize(serialize($ops));
	
	//if(!is_array($ops)) $ops=Array($ops);	
	
	if(isset($ops[/*key($ops)*/0]->embeded)){
		//$ops=dc($ops);//может можно будет обойтись и без этого (проверять на форме комментариев)
		walk_embeded($ops);
		//echo '<br><Br><pre>';
		//var_dump($ops);
		//echo '</pre><br><br>';
	} else if(!empty($ops[/*key($ops)*/0]->embed)){
		walk_embeded($ops);
	}	
	
	$owner_operation='';
	$op=array_shiftA($ops);
	//$iz=0;
	while($op/*=array_shiftA($ops)*/){
		/*$iz++;
		if($iz>20){
			echo '<pre>';
			var_dump($ops);
			echo '</pre>';
			echo '<br><Br>';
			echo $op->data;
			exit;
		}*/
		
		//проверка на embed
		/*if(isset($ops[key($ops)]->embeded)){
			//$ops=dc($ops);//может можно будет обойтись и без этого (проверять на форме комментариев)
			walk_embeded($ops);
		} else if(!empty($ops[key($ops)]->embed)){
			walk_embeded($ops);
		}*/
		
		if(isset($op->mutator)) $op_mutator=$op->mutator;	 else $op_mutator='';
		//if(isset($op->next_operetion)) $op_operation=$op->next_operation;
		
		//$special_compare_op=(isset($ops[key($ops)]) && $ops[key($ops)]->type==LEXER_SUB && $ops[key($ops)]->sub[0]->next_operation==':');						
		$special_compare_op=($op->type==LEXER_SUB && $op->sub[0]->next_operation==':');
		
		if($special_compare_op && !empty($op->sub[0]->data) && $op->sub[0]->data=='uin'){
			$row_id=getrowval("SELECT row_id FROM main_row WHERE row_uin='".$op->sub[1]->data."'",'row_id');
			if(!empty($row_id)){
				seek_rlink($row_id);
				global $rlink;
				if(empty($rlink[$row_id])) return false;
				return $rlink[$row_id];
			} else return false;
		}
		
		if(count($ops)==0){
			if(!is_object($op)) $op=new lexer_op();
			if($op->next_operation=='++' || $op->next_operation=='--'){
				if($op->next_operation=='++') $op->next_operation='+=';
				if($op->next_operation=='--') $op->next_operation='-=';
				$nop=new lexer_op();
				$nop->type=LEXER_NUM;
				$nop->data='1';
				$ops[]=$nop;
				$r=check_operations($owner,$owner_data,$op,$ops/*,$continue*/);
				return $r;
			}
			$op->next_operation='';			
		}
			
		// поддержка базовых операций и устаревших выражений
		if(/*empty($owner) && */isset($op->next_operation) && isset($operations_a[$op->next_operation])){
			//$continue=false;
			$r=check_operations($owner,$owner_data,$op,$ops/*,$continue*/);
			//if(!$continue){
			//etf('parse_var');
			return $r;
			//}
		}
		
		// Блокировка разбора условий, после их очевидного выполнения/не выполнения
		if(($owner_operation=='and' || $owner_operation=='&&') && !$owner_data){
			/*$data=false;
			
			$owner_data=$data;
			if(!isset($op->next_operation)) $op->next_operation='';
			if($op->next_operation=='.'){
				if(isset($op_cmd)) $owner_cmd_type=$op_cmd->type; else $owner_cmd_type=CMD_BASE;
				$owner=$op;
			} else {
				$owner=0;
				$owner_cmd_type=CMD_BASE;
			}
			$owner_operation=$op->next_operation;
			$op=array_shiftA($ops);
			
			continue;*/
			return false;
		}
		if(($owner_operation=='or' || $owner_operation=='||') && $owner_data){
			/*$data=true;

			$owner_data=$data;
			if(!isset($op->next_operation)) $op->next_operation='';
			if($op->next_operation=='.'){
				if(isset($op_cmd)) $owner_cmd_type=$op_cmd->type; else $owner_cmd_type=CMD_BASE;
				$owner=$op;
			} else {
				$owner=0;
				$owner_cmd_type=CMD_BASE;
			}
			$owner_operation=$op->next_operation;
			$op=array_shiftA($ops);

			continue;*/
			return true;
		}
		
		// Тип операции
		if($op->type==LEXER_SUB){
			$data=parse_var($op->sub);
			//if(isset($op->is_array) && !is_array($data) && count($op->sub)==1) $data=Array($data);//лишний isset, но что делать... пока это единственный способ нормально распарсивать еденичные массивы
		}
		if($op->type==LEXER_NUM){
			$data=/*intval(*/$op->data/*)*/;
		}
		if($op->type==LEXER_STR) $data=/*strval(*/$op->data/*)*/;
		if($op->type==LEXER_CMD){
			if(!isset($op->data)) $op->data='';			
			$op_cmd=find_cmd($op->data,$owner_cmd_type,$owner_data,$owner);
			$data=do_cmd($op,$ops,$op_cmd,$owner_data);
		}
	
		// Разбор массива
		if(!empty($op->arr)) foreach($op->arr AS $arr){
		//if(!empty($ops[key($ops)]->arr)) foreach($ops[key($ops)]->arr AS $arr){ //необходимо как-то добавить поддержку поиска именно текущего элемента и его массива, иначе из-за shift_op мы получаем ситуацию, когда массив находится в удаляемой области
			if(is_object($data) && isset($data->rows)) $data=$data->rows;
			if(is_array($data)){
				$fop=first_deep_op($arr);
				if(isset($fop->mutator) && $fop->mutator==MUTATOR_SUP){
					$id=parse_var($arr);
					if(is_object($id) && isset($id->id)) $id=$id->id;
					//поиск по ID строки
					foreach($data AS $d){
						if(is_object($d) && isset($d->id) && $d->id==$id){
							$data=$d;
							break;
						}
					}
				} else {
					//небольшая оптимизация для статических индексов
					if(is_array($arr) && count($arr)==1 && ($arr[0]->type==LEXER_NUM || $arr[0]->type==LEXER_STR)) $arr=$arr[0]->data;
					else $arr=parse_var($arr);
					parse_id($arr);
					if(isset($data[$arr])) $data=$data[$arr]; else $data=false;
				}
			} else if(is_string($data)){
				// Выдаём символ строки по номеру (начиная с 0)
				// небольшая оптимизация для статических индексов
				if(is_array($arr) && count($arr)==1 && ($arr[0]->type==LEXER_NUM || $arr[0]->type==LEXER_STR)) $arr=$arr[0]->data;
				else $arr=parse_var($arr);
				parse_id($arr); //очень странно
				if(!is_object($arr) && strlen($data)>$arr && is_numeric($arr)){
					if(isset($data[$arr])) $data=$data[$arr]; else $data=false;
				}
			} else $data=false;
		}
		
		// Применение мутаторов
		if(!empty($op_mutator)){
			if(empty($ops) || $op->next_operation!='.'){
				if($op_mutator==MUTATOR_MINES){
					if(is_object($data) && isset($data->id)) $data=$data->id;
					if(!is_object($data) && !is_array($data)) $data=$data*(-1);
				}
				if($op_mutator==MUTATOR_NOT){
					$data=!$data;
				}
			} else {
				$do_mutate=$op_mutator;
			}
		}

		//if(!isset($op_cmd)) $cmd->type=CMD_BASE;
		if(!empty($ops)){
			if($op->next_operation=='.' && $op->type==LEXER_NUM && $ops[key($ops)]->type==LEXER_NUM){
				$data=floatval($op->data.'.'.current($ops)->data);
				$op=array_shiftA($ops);
				if(isset($op_mutator) && $op_mutator==MUTATOR_MINES) $data=$data*-1;
			}
			/*if($op->next_operation=='.'){
				if(isset($op_cmd)) $data=parse_var($ops,$op,$data,$op_cmd->type);
				else $data=parse_var($ops,$op,$data);
				// Применение мутаторов для комбинированных операций с точкой
				if(isset($op_mutator) && $op->next_operation=='.'){
					if($op_mutator==MUTATOR_MINES) $data=$data*(-1);
					if($op_mutator==MUTATOR_NOT){
						$data=!$data;
					}
				}
				//etf('parse_var');
				return $data;
			}*/
		}
		
		// логические условия
		if($owner_operation=='and' || $owner_operation=='&&'){
			$data=($owner_data && $data);
		}
		if($owner_operation=='or' || $owner_operation=='||'){
			$data=($owner_data || $data);
		}
		// логические выражения
		if($owner_operation=='=='){
			/*parse_id($owner_data); parse_id($data);
			if(empty($owner_data) && empty($data)){
				$data=true;
			} else if((is_string($owner_data) || is_string($data)) && !is_object($owner_data) && !is_array($owner_data) && !is_object($data) && !is_array($data)){
				$data=((string)$owner_data==(string)$data);
			} else {
				$data=($owner_data==$data);
			}*/
			$data=seek_equal($owner_data,$data);
		}
		if($owner_operation=='!='){
			parse_id($owner_data); parse_id($data);
			$data=($owner_data!=$data);
		}
		if($owner_operation=='<'){
			parse_id($owner_data); parse_id($data);
			$data=($owner_data<$data);
		}
		if($owner_operation=='<='){
			parse_id($owner_data); parse_id($data);
			$data=($owner_data<=$data);
		}
		if($owner_operation=='>='){
			parse_id($owner_data); parse_id($data);
			$data=($owner_data>=$data);
		}
		if($owner_operation=='>'){
			parse_id($owner_data); parse_id($data);
			$data=($owner_data>$data);
		}
		if($owner_operation=='^'){
			parse_id($owner_data); parse_id($data);
			if(is_string($owner_data) && is_string($data) && !empty($owner_data) && !empty($data)){
				$res=strpos($data,$owner_data);
				//$res=strpos($owner_data,$data);
				if($res===false) $data=false; else $data=true;
			} else {
				//вероятно, нужно бэкапить ops перед $data=parse_var($ops); (особенно если вначале parse_var будет снесён dc(ops) )
				if(is_array($data) && count($data)==2 && $special_compare_op){
					//$owner_data=seek_id($owner_data,$data[0],$data[1]);
					$col_id='';
					$col_sname='';
					if(is_numeric($data[1])) $col_id=$data[1];
					if(is_string($data[1])) $col_sname=$data[1];
					if(is_object($data[1]) && isset($data[1]->id)) $col_id=$data[1]->id;
					$data=seek_id($owner_data,$data[0],$col_sname,$col_id);
				} else $data=seek_id($owner_data,$data,'');
			}
		}
		// математические выражения 1ого порядка
		if($owner_operation=='+'){
			//parse_id($owner_data); parse_id($data);
			if(is_object($owner_data) && isset($owner_data->rows)) $owner_data=$owner_data->rows;
			if(is_object($data) && isset($data->rows)) $data=$data->rows;
			if(is_string($owner_data) && is_string($data) && (!is_numeric($owner_data) || !is_numeric($data))) $data=$owner_data.$data;
			else if(is_string($owner_data) && is_object($data) && !isset($data->rows) && isset($data->id)) $data=$owner_data.$data->id;
			else if(is_string($data) && is_object($owner_data) && !isset($owner_data->rows) && isset($owner_data->id)) $data=$owner_data->id.$data;
			else {
				if(is_array($owner_data) && is_object($data) && isset($data->rows)){
					$data=array_merge_recursive($owner_data,$data->rows);
				} else if(is_array($data) && is_object($owner_data) && isset($owner_data->rows)){
					$data->rows=array_merge_recursive($owner_data->rows,$data);
				} else if(!empty($owner_data->rows) && !empty($data->rows)){
					$data->rows=array_merge_recursive($owner_data->rows,$data->rows);
				} else if(is_array($owner_data) && !is_array($data) && (!is_object($data) || !isset($data->rows))){
					if(!empty($data)) $owner_data[]=$data;
					$data=$owner_data;
				} else if(is_object($owner_data) && isset($owner_data->rows) && !is_array($data) && (!is_object($data) || !isset($data->rows))){
					if(!empty($data)) $owner_data->rows[]=$data;
					$data=$owner_data;
				} else if(is_array($data) && !is_array($owner_data) && (!is_object($owner_data) || !isset($owner_data->rows))){
					if(!empty($owner_data)) $data[]=$owner_data;
					//$owner_data=$data;
				} else if(is_object($data) && isset($owner_data->rows) && !is_array($owner_data) && (!is_object($owner_data) || !isset($owner_data->rows))){
					if(!empty($owner_data)) $data->rows[]=$owner_data;
					//$owner_data=$data;
				} else if(is_array($owner_data) && is_array($data)){
					$data=array_merge_recursive($owner_data,$data);
				} else {
					if(is_numeric($owner_data) && is_numeric($data)) $data=$owner_data+$data;
					else if(is_numeric($owner_data) && is_string($data)) $data=$owner_data.$data;
					else if(is_numeric($data) && is_string($owner_data)) $data=$owner_data.$data;
					else if(is_numeric($owner_data) && !is_numeric($data)) $data=$owner_data;
					else if(!is_numeric($owner_data) && is_numeric($data)) $data=$data;
				}
			}
		}
		if($owner_operation=='-'){
			parse_id($owner_data); parse_id($data);
			$data=$owner_data-$data;
		}
		if($owner_operation=='++'){
			parse_id($owner_data); parse_id($data);
			$data=$owner_data.$data;
		}
		if($owner_operation=='+++'){
			parse_id($owner_data); parse_id($data);
			if(!is_numeric($owner_data)) $owner_data=0;
			if(!is_numeric($data)) $data=0;
			$data=$owner_data+$data;
		}
		// математические выражения 2ого порядка
		if($owner_operation=='*'){
			parse_id($owner_data); parse_id($data);
			$data=$owner_data*$data;
		}
		if($owner_operation=='**'){
			parse_id($owner_data); parse_id($data);
			$data=pow($owner_data,$data);
		}
		if($owner_operation=='/'){
			parse_id($owner_data); parse_id($data);
			if($data!=0) $data=$owner_data/$data; else $owner_data=0;
		}
		if($owner_operation=='//'){
			parse_id($owner_data); parse_id($data);
			if($data!=0) $data=floor($owner_data/$data); else $owner_data=0;
		}
		if($owner_operation=='%'){
			parse_id($owner_data); parse_id($data);
			if($data!=0) $data=$owner_data % $data; else $owner_data=0;
		}
		// разделитель 2ого порядка
		if($owner_operation==':'){
			$data=Array($owner_data,$data);
		}
		// создатель массивов
		if($owner_operation==','){
			if(!is_array($data) && !is_array($owner_data)){
				$data=Array($owner_data,$data);
			} else {
				if(is_array($owner_data)){
					$owner_data[]=$data;//возможно тут тоже есть смысл поменять на array_unshift
					$data=$owner_data;
				} else {
					array_unshift($data,$owner_data);
					//$owner_data=$data;
				}
			}
		}		
		
		$owner_data=$data;
		if(!isset($op->next_operation)) $op->next_operation='';
		if(/*$ops[key($ops)]*/$op->next_operation=='.'){
			if(isset($op_cmd)) $owner_cmd_type=$op_cmd->type; else $owner_cmd_type=CMD_BASE;
			$owner=$op;
		} else {
			$owner=0;
			$owner_cmd_type=CMD_BASE;
		}
		$owner_operation=/*$ops[key($ops)]*/$op->next_operation;
		$op=array_shiftA($ops);
		
	}
	if(isset($do_mutate)){
		if($do_mutate==MUTATOR_MINES) $data=$data*(-1);
		if($do_mutate==MUTATOR_NOT){
			$data=!$data;
		}
	}
	//etf('parse_var');
	return $data;
}

?>