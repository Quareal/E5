<?php

// Параметры
define("TPL_CACHE",FTEMP.'tpl_struct/');

// Типы базового вывода
define('TPL_STATIC',1);
define('TPL_DYNAMIC',2);

// Типы операций
define('TPL_PROC',3);
define('TPL_TREE',4);
define('TPL_WHILE',5);
define('TPL_IF',6);
define('TPL_HEAD',7);
define('TPL_BLOCK',8);
define('TPL_SWITCH',9);
define('TPL_SILENT',10);
define('TPL_COMPILE',11);
define('TPL_CLOSE',200); //завершение операции [/if], [/tree] и т.д.

// Типы команд
define('LEXER_CMD',1); // команда
define('LEXER_NUM',2); // число
define('LEXER_STR',3); // строка
define('LEXER_SUB',4); // контейнер с вложениями
define('LEXER_EVAL',5); // исполняемое вложение {}

// Типы мутаторов
define('MUTATOR_MINES',1);// - ; умножение на -1
define('MUTATOR_NOT',2);// ! ; логическое not
define('MUTATOR_SUP',3);// ^ ; модификатор индекса массива (искать по ID, вместо ключа) / либо прочих команд
define('MUTATOR_SPEC',4);// ~ ;
//define('MUTATOR_LINE',5);// подчёркивание перед оператором
//define('MUTATOR_PARAM',6);// модификатор переменных - $

// Типы вставок
define('EMBED_TPL',0);
define('EMBED_PARSE',1);
define('EMBED_STRING',2);
define('EMBED_IMPRINT',3);

class parse_item{
	public $main='';
	public $type=TPL_PROC;
}

class lexer_op{
	public $type=LEXER_CMD;
	public $next_operation='';
	public $priority=0;
	public function __clone(){
		//if(!empty($this->put)) foreach($this->put AS $t) $t=clone $t;
		//if(!empty($this->sub)) foreach($this->sub AS $t) $t=clone $t;
		/*if(!empty($this->arr))  foreach($this->arr AS $a) foreach($a AS $t){$t=clone $t;}
		if(!empty($this->embed)) foreach($this->embed AS $t) $t=clone $t;*/
	}
}

class lexer_search{
	public $priority;
	public $operation;
}

function init_lexer(){
	global $lex_spaces, $lex_split, $lex_search, $lex_hash, $lex_ini_status, $lex_except, $lex_logical, $lex_math, $lex_equal, $lex_func_sep;
	if(!empty($lex_ini_status)) return false;
	$lex_ini_status=1;
	// Определение допустимых пробелов
	$lex_spaces=Array(/*',',*/' ','	',chr(10),chr(13));
	// Определение разделителей
	$lex_split[0]=$lex_spaces;
	$lex_split[1]=Array('|');
	$lex_split[2]=Array(',');
	$lex_split[3]=Array('+>','->','=>','..','*>','+=','-=','*=','/=','=');
	$lex_split[4]=Array('or','||');
	$lex_split[5]=Array('and','&&');
	$lex_split[6]=Array('==','!=','<','<=','>=','>','^', /* для fastget запросов */'!^^','^^','!^','^!','!^!');
	$lex_split[7]=Array(':');
	$lex_split[8]=Array('+','-','--','++','+++');
	$lex_split[9]=Array('*','**','/','//','%');
	$lex_split[10]=Array('.');
	$lex_except=array_flip(Array('and','or')); // Данным разделителям запрещено прилегать к операторам
	$lex_logical=array_flip(array_merge($lex_split[4],$lex_split[5],$lex_split[6]));
	$lex_equal=array_flip($lex_split[6]);
	$lex_math=array_flip(array_merge($lex_split[8],$lex_split[9]));
	$lex_func_sep=array_merge($lex_split[0],$lex_split[2]);
	// Массив поиска по разделителям
	$lex_spaces=array_flip($lex_spaces);
	$lex_search=Array();
	foreach($lex_split AS $index=>$obj){
		foreach($obj AS $o){
			$lex_search[$o]=new lexer_search();
			$lex_search[$o]->priority=$index;
			$lex_search[$o]->operation=$o;
		}
	}
	// Хеш лексера
	//lex hash нужен для того, чтобы перестраивать результат работы лексера в том случае, если правила лексера изменились	
	$lex_hash=md5(serialize($lex_split)); // возможно имеет смысл вместо него использовать размер исполняемого файла лексера
}

function start_parse_tpl($tpl_data,$ignore_read=0,$ignore_write=0){
	// инициализация парсера
	init_lexer();
	global $lex_hash;
	$test=false;
	//if(strpos($tpl_data,'after')) $test=true;
	// проверка на наличие распарсенной информации в кеше
	$tpl_hash=md5($tpl_data.$lex_hash);
	if(!$test && !$ignore_read && file_exists(TPL_CACHE.$tpl_hash)) return unserialize(file_get_contents(TPL_CACHE.$tpl_hash));
	// начало работы парсера
	global $tpl,$tpl2,$caret,$length;
	$tpl=$tpl_data;
	$tpl2=strtolower($tpl_data);
	$caret=0;
	$length=strlen($tpl);
	$result->main='';
	$result->elems=Array();
	parse_tpl($result);
	if($test){
		echo '<pre>';
		var_dump($result);
		echo '</pre>';
	}
	// запись результатов
	if(!$ignore_write){
		check_dir(TPL_CACHE);
		$f=fopen(TPL_CACHE.$tpl_hash,'w');
		fwrite($f,serialize($result));
		fclose($f);
	}
	return $result;
}

function parse_tpl(&$item,$wait_for=Array(),$wait_for_symb=''){
	global $caret, $tpl, $length;
	if(!empty($wait_for)) $wait_for=array_flip($wait_for);	
	if(!isset($item->elems)){
		$item->elems=Array();
		$count=0;
	} else $count=count($item->elems);	
	$elem=&$item->elems[$count];
	$elem->type=TPL_STATIC;
	$elem->data='';
	$count++;	
	// Парсим настройки кеша
	if(!empty($tpl[$caret]) && $tpl[$caret]=='^'){
		$caret++;
		$item->cache=$tpl[$caret];
		$caret++;
		if(isset($tpl[$caret]) && $tpl[$caret]=='~'){
			$item->cache_ignore_user=true;			
			$caret++;
		}
		if(isset($tpl[$caret]) && $tpl[$caret]=='@'){
			$item->cache_no_del=true;			
			$caret++;
		}
		if(isset($tpl[$caret]) && $tpl[$caret]=='!'){
			$item->cache_host=true;			
			$caret++;
		}
		if(isset($tpl[$caret]) && $tpl[$caret]=='('){
			$item->cache_time='';
			$caret++;
			if(isset($tpl[$caret]) && $tpl[$caret]=='$'){
				$item->cache_in_day=1;
				$caret++;
			}
			while(isset($tpl[$caret]) && $tpl[$caret]!=')'){
				$item->cache_time.=$tpl[$caret];
				$caret++;
			}
			$caret++;
		}
	}
	$open_comment=0;
	$open_noscript=0;
	$old_caret=$caret;
	for($caret=$old_caret;$caret<$length;$caret++){	
		$x=$tpl[$caret];
		if(!empty($tpl[$caret+1])) $y=$tpl[$caret+1]; else $y='';
		// Определение точки возврата
		if($x==$wait_for_symb && !$open_comment && !$open_noscript){
			break;
		}
		// Определение NoScript элементов
		if($x.$y=='[~' && !$open_comment){ $open_noscript++; $caret++; }
		else if($x.$y=='~]' && !$open_comment && $open_noscript){ $open_noscript--; $caret++; }
		// Игнорирование комментариев
		else if($x.$y=='[*' && !$open_noscript){ $open_comment++; $caret++; }
		else if($x.$y=='*]' && !$open_noscript && $open_comment){ $open_comment--; $caret++; }
		// Разбор выражения
		else if($x=='[' && $y!='[' && !$open_comment && !$open_noscript){
			$caret++;			
			// Создаём новый элемент, либо используем первый, если он пустой
			if($item->elems[$count-1]->type==TPL_STATIC && $item->elems[$count-1]->data==''){
				$elem=&$item->elems[$count-1];
			} else {
				$elem=&$item->elems[$count];
				$count++;
			}			
			// Парсим динамическую составляющую
			$elem->type=TPL_DYNAMIC;
			$elem->dynamic=parse_tpl_step($elem);
			if($elem->dynamic===false){
				array_pop($item->elems);
			}
			if(!empty($wait_for) && isset($elem->dynamic->main) && isset($wait_for[$elem->dynamic->main])){ // например wait_for in 'else','/if' или же 'after','/tree', или '/head'
				$item->main=$elem->dynamic->main;
				if(!empty($elem->dynamic->ops) && count($elem->dynamic->ops)>1){
					if(isset($elem->dynamic->ops[1]->sub)){
						$item->ops=$elem->dynamic->ops[1]->sub;
					} else {
						array_shift($elem->dynamic->ops);
						$item->ops=$elem->dynamic->ops;
					}
				}
				array_pop($item->elems);
				if(isset($item->cache)) $item->cache_md5=md5(substr($tpl,$old_caret,$caret-$old_caret));
				return $item;
			}			
			// Создаём новый текстовый элемент для дальнейшей записи
			$elem=&$item->elems[$count];
			$elem->type=TPL_STATIC;
			$elem->data='';
			$count++;				
		} else if(!$open_comment) {
			// Преобразуем двойные скобки в одинарные
			if($x=='[' && $y=='[' && !$open_noscript){ $caret++; }
			if($x==']' && $y==']' && !$open_noscript){ $caret++; }
			$item->elems[$count-1]->data.=$x;
		}
	}
	if($item->elems[$count-1]->data=='') array_pop($item->elems);
	if(isset($item->cache)) $item->cache_md5=md5(substr($tpl,$old_caret,$caret-$old_caret));
	return $item;
}

function parse_tpl_step(&$elem){
	global $caret, $lex_spaces, $tpl;
	$res=new parse_item();
	$res->type=TPL_PROC;
	$res->ops=lexer();
	$caret++;
	if(empty($res->ops)) return false;
	$first=$res->ops[0];
	if(isset($res->ops[0]->data)) $res->main=$res->ops[0]->data;
	else $res->ops[0]->data='';
	$tmp_r=new parse_item();	
	// Парсинг базовых операций (циклы, условия)
	if($first->type==LEXER_CMD && count($res->ops)>0){
		$cmd=strtolower($first->data);
		if($cmd=='tree'){
			$res->type=TPL_TREE;
			$caret++;//?
			if($res->ops[1]->type==LEXER_SUB && $res->ops[1]->sub[0]->next_operation=='..'){
				$cmd='for';
				$close_box='/tree';
				$caret--;
			} else {
				$tmp=parse_tpl($tmp_r,Array('after','/tree'));
				$tmp_r=new parse_item();
				$res->body=$tmp;
				if($tmp->main=='after'){
					$caret++;//?
					$tmp=parse_tpl($tmp_r,Array('/tree'));
					$res->after=$tmp;
					$tmp_r=new parse_item();
				} else $res->after='';
				array_shift($res->ops);
				$res->tree_type=0;
				$res->tree_deep=0;
				$res->tree_rev=false;
				$res->tree_rand=false;
				if(isset($lex_spaces[$res->ops[0]->next_operation])){
					$t_ops=$res->ops;
					foreach($t_ops AS $o){
						if(isset($o->data) && $o->data=='linear'){
							$res->tree_deep=1;
						} else if(isset($o->data) && $o->data=='rev'){
							$res->tree_rev=true;
						} else if(isset($o->data) && $o->data=='rand'){
							$res->tree_rand=true;
						} else if(isset($o->data) && $o->data=='deep'){
							$res->tree_deep=$o->put[0];
						} else $res->ops=$o->sub;
					}
				}
			}
		}
		if($cmd=='for'){
			$res->type=TPL_TREE;
			$caret++;//?
			if(!empty($close_box)){
				$tmp=parse_tpl($tmp_r,Array($close_box));
				$close_box='';
			} else $tmp=parse_tpl($tmp_r,Array('/for'));
			$tmp_r=new parse_item();
			$res->body=$tmp;
			$res->tree_type=1;
			$res->tree_start=$res->ops[1]->sub[0];
			$res->tree_end=$res->ops[1]->sub[1];
			array_shift($res->ops);
		}
		if($cmd=='while'){
			$res->type=TPL_WHILE;
			$caret++;//?
			$tmp=parse_tpl($tmp_r,Array('/while'));
			$tmp_r=new parse_item();
			$res->body=$tmp;
			array_shift($res->ops);
		}
		if($cmd=='compile'){
			$res->type=TPL_COMPILE;
			$caret++;//?
			$start_copy=$caret;			
			$tmp=parse_tpl($tmp_r,Array('/compile'));
			$end_copy=$caret-strlen('[/compile]');
			$tmp_r=new parse_item();
			$res->body=$tmp;
			array_shift($res->ops);
			$res->txt_body=substr($tpl,$start_copy,$end_copy-$start_copy+1);
		}
		if($cmd=='switch'){
			$res->type=TPL_SWITCH;
			$caret++;//?
			$res->cases=Array();
			$from='';
			$f_op=Array();
			$tmp=parse_tpl($tmp_r,Array('case','default','/switch'));
			$tmp_r=new parse_item();
			while($tmp->main=='case'){
				$caret++;
				if(empty($from)){
					$from='case';
					$f_op=$tmp->ops;
				} else {
					$res->cases[]=Array('case'=>$f_op,'body'=>$tmp);
					$f_op=$tmp->ops;
				}
				$tmp=parse_tpl($tmp_r,Array('case','default','/switch'));
				$tmp_r=new parse_item();
			}
			if($tmp->main=='default'){
				$caret++;
				if($from=='case'){
					$res->cases[]=Array('case'=>$f_op,'body'=>$tmp);
				}
				$from='default';
				$tmp=parse_tpl($tmp_r,Array('case','default','/switch'));
				$tmp_r=new parse_item();
			}
			if($tmp->main=='/switch'){
				if($from=='case'){
					$res->cases[]=Array('case'=>$f_op,'body'=>$tmp);
				}
				if($from=='default') $res->default=$tmp;
			}
			array_shift($res->ops);
			//var_dump($res->cases);
		}
		if($cmd=='block' && $res->ops[0]->next_operation!='.'){
			$res->type=TPL_BLOCK;
			$caret++;//?
			$tmp=parse_tpl($tmp_r,Array('/block'));
			$tmp_r=new parse_item();
			$res->body=$tmp;
			array_shift($res->ops);
		}
		if($cmd=='if' && (empty($first->put) || count($first->put)==1 || $first->put[0]->next_operation!=',')){
			$res->type=TPL_IF;
			$caret++;//?
			$tmp=parse_tpl($tmp_r,Array('else','elseif','/if'));	
			$tmp_r=new parse_item();
			$res->body=$tmp;
			while($tmp->main=='elseif'){
				$caret++;//?
				$case=$tmp->ops;
				$tmp=parse_tpl($tmp_r,Array('else','elseif','/if'));
				$res->elseif[]=Array('case'=>$case,'body'=>$tmp);
				$tmp_r=new parse_item();//?
			}
			if($tmp->main=='else'){
				$caret++;//?
				$tmp=parse_tpl($tmp_r,Array('/if'));
				$res->else=$tmp;
				$tmp_r=new parse_item();
			} else $res->else='';
			if(!empty($first->put) && count($res->ops))  $res->ops=$first->put;
			else array_shift($res->ops);
		}
		if($cmd=='head'){
			$res->type=TPL_HEAD;
			$caret++;//?
			$start_copy=$caret;
			$res->body=parse_tpl($tmp_r,Array('/head'));
			$end_copy=$caret-strlen('[/head]');// возможно нужно добавить -1 или +1
			$res->txt_body=substr($tpl,$start_copy,$end_copy-$start_copy+1);
		}
		if($cmd=='silent'){
			$res->type=TPL_SILENT;
			$caret++;//?
			$tmp=parse_tpl($tmp_r,Array('/silent'));
			$tmp_r=new parse_item();
			$res->body=$tmp;
			array_shift($res->ops);
		}
		if($cmd=='/tree'){
			$res->main='/tree';
			$res->type=TPL_CLOSE;
		}
		if($cmd=='/for'){
			$res->main='/for';
			$res->type=TPL_CLOSE;
		}
		if($cmd=='/while'){
			$res->main='/while';
			$res->type=TPL_CLOSE;
		}
		if($cmd=='after' && empty($first->put) && empty($first->next_operation)){
			$res->main='after';
			$res->type=TPL_CLOSE;
		}
		if($cmd=='/if'){
			$res->main='/if';
			$res->type=TPL_CLOSE;
		}
		if($cmd=='else'){
			$res->main='else';
			$res->type=TPL_CLOSE;
		}
		if($cmd=='/head'){
			$res->main='/head';
			$res->type=TPL_CLOSE;
		}
		if($cmd=='/switch'){
			$res->main='/switch';
			$res->type=TPL_CLOSE;
		}
		if($cmd=='/silent'){
			$res->main='/silent';
			$res->type=TPL_CLOSE;
		}
	}
	return $res;
}

// Определение типа этапа
function finalize_op(&$op){
	global $lex_split;
	if(!isset($op->data) && !empty($op->sub)) $op->type=LEXER_SUB;
	if($op->priority==-1) $op->priority=count($lex_split)-1;
	if(!isset($op->data)) return false; //т.к. дальше идёт обработка значения
	if(is_numeric($op->data)) $op->type=LEXER_NUM;
	if(strlen($op->data)>0){
		if($op->data[0]=='-'){
			$op->data=substr($op->data,1);
			if(!isset($op->mutator)) $op->mutator=MUTATOR_MINES;
		}
		if($op->data[0]=='!'){
			$op->data=substr($op->data,1);
			if(!isset($op->mutator)) $op->mutator=MUTATOR_NOT;
		}
		if($op->data[0]=='^'){
			$op->data=substr($op->data,1);
			if(!isset($op->mutator)) $op->mutator=MUTATOR_SUP;
		}
		if($op->data[0]=='~'){
			$op->data=substr($op->data,1);
			if(!isset($op->mutator)) $op->mutator=MUTATOR_SPEC;
		}
		//if($op->data[0]=='_'){
		//	$op->data=substr($op->data,1);
		//	$op->mutator=MUTATOR_LINE;
		//}
		//if($op->data[0]=='$'){
			//$op->data='param';//substr($op->data,1);
			//$op->mutator=MUTATOR_PARAM;
		//}
		if(is_numeric($op->data)) $op->type=LEXER_NUM;
		if(($op->data[0]=='"' && $op->data[strlen($op->data)-1]=='"') || ($op->data[0]=="'" && $op->data[strlen($op->data)-1]=="'")){
			if($op->data[0]=='"') $op->str_type=0; else $op->str_type=1;
			$op->data=substr($op->data,1,strlen($op->data)-2);
			$op->type=LEXER_STR;
		}
		if($op->type==LEXER_CMD){
			//$op->data=str_replace('\'','',$op->data);
			$op->data=str_replace('\\','',$op->data);
		}
	}
}

/*function resort_ops(&$ops){
	$next_key='';
	foreach($ops AS $key=>$tmp){
		$op=&$ops[$key];
		if(isset($op->next_operation)) $next_key_tmp=$op->next_operation; else $next_key_tmp='';
		$op->next_operation=$next_key;
		$next_key=$next_key_tmp;
	}
	krsort($ops);
}*/

// Парсер выражения
function lexer($ops=Array(),$wait_for=']',&$owner=0){
	global $caret,$tpl,$tpl2,$length,$lex_split,$lex_spaces,$lex_search,$lex_except;
	$old_caret=$caret;
	$is_string=false;//'
	$is_string2=false;//"
	$is_slash=0;// \
	$prev_safe_space=false;
	$ignore_spaces=true;
	$wait_for_space=false;
	$num_op=0;
	if(empty($ops)){
		$ops=Array();
		$ops[count($ops)]=new lexer_op();
		$cop=&$ops[count($ops)-1];
		$cop->priority=-1;
		$cop->type=LEXER_CMD;
		$cop->next_operation='';
	} else $cop=&$ops[count($ops)-1];	
	for($caret=$old_caret;$caret<$length;$caret++){
		// Определение переменных
		$sep='';
		if($is_slash) $is_slash--;
		if($prev_safe_space) $prev_safe_space--;
		$x=$tpl[$caret];
		$x2=$tpl2[$caret];
		// Определение строки для поиска разделителя (макс. 3 симв, можно оптимизировать)
		if($caret+1<$length) $y=$tpl2[$caret+1]; else $y='0';
		if($caret+2<$length) $z=$tpl2[$caret+2]; else $z='0';
		// Определение символа строки
		if($x=="'" && !$is_string2 && !$is_string && !$is_slash) $is_string=true;
		else if($x=="'" && !$is_string2 && $is_string && !$is_slash) $is_string=false;
		if($x=='"' && !$is_string && !$is_string2 && !$is_slash) $is_string2=true;
		else if($x=='"' && !$is_string && $is_string2 && !$is_slash) $is_string2=false;
		// Имеем ли мы дело с строкой
		$a_string=($is_string || $is_string2);
		// Найден слеш - поиск следующего за ним символа
		if(/*!$a_string && */$x=="\\" && !$is_slash){
			$is_slash=2;
			continue;
		}
		// Обнаружение двойного пробела - продолжение
		if(!$a_string && isset($lex_spaces[$x]) && ($prev_safe_space || $ignore_spaces)){	
			continue;
		}
		// Обнаружение пробела - поиск следующих за ним пробелов
		if(!$a_string && isset($lex_spaces[$x]) && !$prev_safe_space){	
			$prev_safe_space=2;
			$ignore_spaces=false;
			// даёт ошибку при распарсивании шаблона вида [x.y a+b x+z] (операции a,b,x,z идут в одном стеке)
		}
		// Разделитель оператора
		if(!$a_string && $x==';' && empty($owner)){
			finalize_op($cop);
			$tpl[$caret]='[';
			$tpl2[$caret]='[';
			$caret--;
			$caret--;
			//resort_ops($ops);
			return $ops;
		}
		if(!$a_string && $x==';' && !empty($owner)){
			finalize_op($cop);
			$caret--;
			//resort_ops($ops);
			return $ops;
		}
		// Обнаружение искомого - выход
		if(!$a_string && $x==$wait_for){
			if(!isset($cop->data) && empty($cop->sub) && empty($cop->put)){
				array_pop($ops);
			} else {
				finalize_op($cop);
			}
			$caret--;
			//resort_ops($ops);
			return $ops;
		}
		// Обнаружен пробел - идём дальше, чтобы определить - разделитель это или пропуск
		if(!$a_string && isset($lex_spaces[$x])){
			$wait_for_space=true;
			continue;
		}		
		// Ищем разделитель
		if(!$a_string && empty($sep)){
			if(!empty($lex_search[$x2.$y.$z])){
				$sep=$lex_search[$x2.$y.$z];
				$caret+=2;
				$move_caret=3;
				$ignore_spaces=true;
				$num_op++;
			} else if(!empty($lex_search[$x2.$y])){
				$sep=$lex_search[$x2.$y];
				$caret+=1;
				$move_caret=2;
				$ignore_spaces=true;
				$num_op++;
			} else if(!empty($lex_search[$x2])){
				$sep=$lex_search[$x2];
				$move_caret=1;
				$ignore_spaces=true;
				$num_op++;
			} else {
				$ignore_spaces=false;
				$move_caret=0;
			}
		}
		// Преобразуем все виды пробелов к единому
		if(!empty($sep) && isset($lex_spaces[$sep->operation])){
			$sep=$lex_search[' '];
		}
		// Проверка на исключения
		if(!$a_string && !empty($sep) && isset($lex_except[$sep->operation])){
			if(empty($tpl[$caret-$move_caret]) || empty($tpl[$caret+1]) || !isset($lex_spaces[$tpl[$caret+1]]) || !isset($lex_spaces[$tpl[$caret-$move_caret]])){
				$caret=$caret-$move_caret+1;
				$num_op--;
				$ignore_spaces=false;
				unset($sep);
			}
		}
		// Подтверждение пробела, как пропуска
		if(!$a_string && $wait_for_space && !empty($sep)){
			$wait_for_space=false;
		}
		// Подтверждение пробела, как разделителя, а не как пропуска
		if($wait_for_space && empty($sep)){
			$caret--;
			$sep=$lex_search[' '];
			$wait_for_space=false;
			$move_caret=1;
			if($is_string) $is_string=false;
			if($is_string2) $is_string2=false;
		}
		// Поддержка старого формата записей в fastget
		if(!empty($sep) && !empty($cop->data) && strstr($cop->data,'fastget') && strlen($cop->data)<12 && $sep->operation==':') $sep=$lex_search['.'];
		// Следующий этап
		if(!empty($sep)){
			if($cop->priority==-1) $cop->priority=$sep->priority;
			if($sep->priority==$cop->priority){
				// Добавляем этап
				// Обработка -1 значений // Перенесено в мутаторы
				//if($num_op==1 && $sep->operation=='-' && empty($cop->data)){
				//	//continue;
				//	$cop->data=-1;
				//	$cop->next_operation='*';
				//	$cop->priority=$lex_search['*']->priority;
				//	finalize_op($cop);
				//	$ops[count($ops)]=new lexer_op();
				//	$cop=&$ops[count($ops)-1];
				//	$cop->priority=$lex_search['*']->priority;
				//	$cop->type=LEXER_CMD;
				//	$cop->next_operation='';
				//	continue;
				//}
				// Обработка минуса
				if($num_op==1 && $sep->operation=='-' && empty($cop->data)){
					$cop->data='-';
					continue;
				}
				// Обработка префикса индекса массивов
				if($num_op==1 && $sep->operation=='^' && empty($cop->data)){
					$cop->data='^';
					continue;
				}
				// Обработка префикса переменных
				//if($num_op==1 && $sep->operation=='$' && empty($cop->data)){
				//	//$cop->data='$';
				//	$cop->data='param';
				//	$tpl[$caret]='.';
				//	$caret--;
				//	continue;
				//}
				// Обработка закрывающих тегов
				if($num_op==1 && $caret-$old_caret==0 && empty($owner) && $sep->operation=='/' && empty($cop->data)){
					$cop->data='/';
					continue;
				}
				$cop->next_operation=$sep->operation;
				finalize_op($cop);
				$ops[count($ops)]=new lexer_op();
				$cop=&$ops[count($ops)-1];
				$cop->priority=$sep->priority;
				$cop->type=LEXER_CMD;
				$cop->next_operation='';
				continue;
			}
			if($sep->priority>$cop->priority){
				// Добавляем в подэтап
				if(!isset($cop->sub)) $cop->sub=Array();
				if($cop->type==LEXER_SUB && !empty($cop->sub)/* && $cop->sub[0]->priority!=$cop->priority*/){
					$safe_sub=$cop->sub;
					$cop->sub=Array();
				} else $safe_sub=Array();
				$cop->sub[count($cop->sub)]=new lexer_op();
				$new_cop=&$cop->sub[count($cop->sub)-1];
				if(isset($cop->data)) $new_cop->data=$cop->data;
				if(!empty($cop->arr)) $new_cop->arr=$cop->arr;
				if(!empty($cop->put)) $new_cop->put=$cop->put;
				if(!empty($cop->embed)){
					$new_cop->embed=$cop->embed;
					unset($cop->embed);
				}
				if(!empty($safe_sub)) $new_cop->sub=$safe_sub;
				$new_cop->priority=$sep->priority;
				$new_cop->type=LEXER_CMD;
				$new_cop->next_operation=$sep->operation;
				finalize_op($new_cop);
				// Создаём новый этап
				$cop->type=LEXER_SUB;
				$cop->arr=Array();
				$cop->put=Array();
				if(isset($cop->data)) unset($cop->data);
				$caret=$caret-$move_caret+1;
				$cop->sub=lexer($cop->sub,$wait_for,$cop);
				$cop->next_operation=-1;	
				$prev_safe_space=0;   //? исправляет косяк с [x+y a+b z+x], который интерпретируется как (x+y), (a,b,z,x) (выше описано)
				$ignore_spaces=false;  //?
				continue;
			}
			if($sep->priority<$cop->priority){
				// Обработка минуса
				if(/*$num_op==1 && */$sep->operation=='-' && empty($cop->data)){
					$cop->data='-';
					continue;
				}
				if(!empty($owner) && $owner->priority>=$sep->priority){
					// Если приоритет следующей операции важнее текущей, и есть родитель - возвращаться на предыдущий этап
					$caret=$caret-$move_caret+1;
					finalize_op($cop);
					$caret--;
					//resort_ops($ops);
					return $ops;
				} else {
					// Если родителя нет - создаём его
					
					// Затея с перемещением Embeded-флага на верх
					if(isset($ops[0]->embeded)){
						$move_embed=1;
						unset($ops[0]->embeded);
					} else $move_embed=0;
					
					finalize_op($cop);
					$sops=$ops;
					$ops=Array();
					$ops[count($ops)]=new lexer_op();
					$cop=&$ops[count($ops)-1];
					$cop->type=LEXER_SUB;
					$cop->sub=$sops;
					$cop->next_operation=$sep->operation;
					$cop->priority=$sep->priority;
					finalize_op($cop);
					$ops[count($ops)]=new lexer_op();
					$cop=&$ops[count($ops)-1];
					$cop->priority=$sep->priority;
					$cop->type=LEXER_CMD;
					$cop->next_operation='';
					// Перемещение embeded-флага на верх
					if($move_embed) $ops[0]->embeded=1;
					continue;					
				}
				continue;
			}
		}		
		// Вложения - скобки и массивы
		if(!$a_string && $x=='['){
			$caret++;
			$cop->arr[]=lexer(Array(),']');
			$caret++;
			continue;
		}
		if(!$a_string && $x=='('){
			$caret++;
			if(!empty($cop->data) && $cop->data!='!' && $cop->data!='^' && $cop->data!='-') $cop->put=lexer(Array(),')');
			else {
				$cop->type=LEXER_SUB;
				$cop->sub=lexer(Array(),')');
				//$cop->is_array=true;//это надо тестить, создано для того, чтобы работать с x=(1) как с массивом, состоящим из одного элемента (иначе - это будет просто 1)
			}
			$caret++;
			continue;
		}		
		// Иначе - добавляем данные к текущей data
		if(!$a_string && (empty($cop->data) || $cop->data=='-' || $cop->data=='!' || $cop->data=='^') && $x=='$'){
			if(!isset($cop->data)) $cop->data='param';
			else $cop->data.='param';//для включения - и !
			$tpl[$caret]='.';
			$tpl2[$caret]='.';
			$caret--;
			//$caret--;
			continue;
		}
		if(!$a_string && $x=='{'){
			$caret++;
			$ops[0]->embeded[]=&$cop;
			$e=&$cop->embed[];
			if(isset($cop->data)) $e->start=strlen($cop->data);
			else $e->start=0;
			$e->type=EMBED_TPL;
			if($y=='^') $e->type=EMBED_PARSE;
			if($y=='%') $e->type=EMBED_STRING;
			if($y=='!' || $y=='@') $e->type=EMBED_IMPRINT;
			if($e->type!=EMBED_TPL) $caret++;
			if($e->type==EMBED_PARSE){
				$e->data=lexer(Array(),'}');
			} else if($e->type==EMBED_IMPRINT){
				$e->data='';
				while($tpl[$caret]!='}'){
					$e->data.=$tpl[$caret];
					$caret++;
				}
			} else {
				$tmp_r=new parse_item();
				$e->data=parse_tpl($tmp_r,Array(),'}');
			}
			if(!isset($cop->data)) $cop->data='';
			$cop->old_data=$cop->data;
			continue;
		}
		if(isset($cop->data)) $cop->data.=$x;
		else $cop->data=$x;
	}
	if(!isset($cop->data) && empty($cop->sub) && empty($cop->put) && empty($cop->embed)){
		array_pop($ops);
	} else finalize_op($cop);
	//resort_ops($ops);
	return $ops;
}

// Вызов лексера для частных выражений
function shell_lexer($lex){
	global $tpl,$tpl2,$caret,$length;
	init_lexer();
	$tpl=$lex;
	$tpl2=strtolower($tpl);
	$caret=0;
	$length=strlen($tpl);
	return lexer();
}

//Необходимо полностью переделать - добавить поддержку мутаторов, добавить над-процедуру компиляции шаблона (текстового) из объекта
function ops_to_tpl($ops=Array()){
	$result='';
	if(empty($ops)) return $result;
	foreach($ops AS $op){
		if(isset($op->mutator)){
			if($op->mutator==MUTATOR_MINES) $result.='-';
			if($op->mutator==MUTATOR_NOT) $result.='!';
			if($op->mutator==MUTATOR_SUP) $result.='^';
		}
		if($op->type==LEXER_SUB && !empty($op->sub)){
			$result.='(';
			$result.=ops_to_tpl($op->sub);
			$result.=')';
		}
		if($op->type==LEXER_CMD){
			$result.=$op->data;
		}
		if($op->type==LEXER_NUM){
			$result.=$op->data;
		}
		if($op->type==LEXER_STR){
			if($op->str_type==0) $result.='"'.$op->data.'"';
			else $result.="'".$op->data."'";
		}
		if(!empty($op->arr)) foreach($op->arr AS $arr){
			$result.='[';
			$result.=ops_to_tpl($arr);
			$result.=']';
		}
		if(!empty($op->next_operation)) $result.=' '.$op->next_operation.' ';
	}
	return $result;
}

// Превращает объект шаблона в текст
function compile_tpl($tpl){
	
}

?>