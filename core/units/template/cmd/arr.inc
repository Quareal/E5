<?php

define('GROUP_ARR_SPEC',3001);
$cmd_group[GROUP_ARR_SPEC]=Array('title'=>'Специфические','pos'=>8);
define('GROUP_ARR_SORT',3002);
$cmd_group[GROUP_ARR_SORT]=Array('title'=>'Сортировка','pos'=>1);
define('GROUP_ARR_FILTER',3003);
$cmd_group[GROUP_ARR_FILTER]=Array('title'=>'Фильтрация','pos'=>2);
define('GROUP_ARR_OPS',3004);
$cmd_group[GROUP_ARR_OPS]=Array('title'=>'Операции','pos'=>3);
define('GROUP_ARR_NEW',3005);
$cmd_group[GROUP_ARR_NEW]=Array('title'=>'Составить массив','pos'=>4);
define('GROUP_ARR_NAV',3006);
$cmd_group[GROUP_ARR_NAV]=Array('title'=>'Навигация','pos'=>5);
define('GROUP_ARR_SUB',3007);
$cmd_group[GROUP_ARR_SUB]=Array('title'=>'Подтаблицы','pos'=>6);

function find_table_in_arr(&$arr){
	$table=0;
	if(!is_object($arr) && isset($arr[0]) && is_object($arr[0]) && !empty($arr[0]->table)) $table=$arr[0]->table;
	if(empty($table) && is_object($arr) && isset($arr->rows) && isset($arr->rows[0]) && is_object($arr->rows[0]) && !empty($arr->rows[0]->table)) $table=$arr->rows[0]->table;
	return $table;
}


$cmd['_function'][CMD_ARRAY]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_func'][CMD_ARRAY]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_component'][CMD_ARRAY]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_show'][CMD_ARRAY]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_form'][CMD_ARRAY]->alias=&$cmd['_function'][CMD_ROW];

$cmd['_part'][CMD_ARRAY]->alias=&$cmd['_function'][CMD_ROW];

//$cmd['_function'][CMD_ARRAY]->visual=Array('title'=>'функции', 'special'=>SP_COMPONENT,'extend_put'=>1);
//$cmd['_show'][CMD_ARRAY]->visual=Array('title'=>'отображения', 'special'=>SP_COMPONENT,'extend_put'=>1);
//$cmd['_component'][CMD_ARRAY]->visual=Array('title'=>'компоненты', 'special'=>SP_COMPONENT,'extend_put'=>1);
//$cmd['_form'][CMD_ARRAY]->visual=Array('title'=>'формы', 'special'=>SP_COMPONENT,'extend_put'=>1);

/*$cmd['_function'][CMD_ARRAY]->visual=Array('title'=>'функции','put'=>Array(0=>Array('title'=>'функция','type'=>STRING_FUNCTION_NAME,'req'=>1)), 'special'=>SP_POINT,'extend_put'=>1);
$cmd['_show'][CMD_ARRAY]->visual=Array('title'=>'отображения','put'=>Array(0=>Array('title'=>'отображение','type'=>STRING_SHOW_NAME,'req'=>1)), 'special'=>SP_POINT,'extend_put'=>1);
$cmd['_component'][CMD_ARRAY]->visual=Array('title'=>'компоненты','put'=>Array(0=>Array('title'=>'компонент','type'=>STRING_COMPONENT_NAME,'req'=>1)), 'special'=>SP_POINT,'extend_put'=>1);
$cmd['_form'][CMD_ARRAY]->visual=Array('title'=>'формы','put'=>Array(0=>Array('title'=>'форма','type'=>STRING_FORM_NAME,'req'=>1)), 'special'=>SP_POINT,'extend_put'=>1);*/

$cmd['_function'][CMD_ARRAY]->visual=Array('title'=>'функции', 'special'=>SP_COMPONENT,'extend_put'=>1,'group'=>GROUP_ARR_SPEC);
$cmd['_show'][CMD_ARRAY]->visual=Array('title'=>'отображения', 'special'=>SP_COMPONENT,'extend_put'=>1,'group'=>GROUP_ARR_SPEC);
$cmd['_component'][CMD_ARRAY]->visual=Array('title'=>'компоненты', 'special'=>SP_COMPONENT,'extend_put'=>1,'group'=>GROUP_ARR_SPEC);
$cmd['_form'][CMD_ARRAY]->visual=Array('title'=>'формы', 'special'=>SP_COMPONENT,'extend_put'=>1,'group'=>GROUP_ARR_SPEC);

$cmd['_part'][CMD_ARRAY]->visual=Array('title'=>'части', 'special'=>SP_PART,'extend_put'=>1,'group'=>GROUP_ARR_SPEC);


$cmd['e5'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_array($arr) && !empty($arr)) return seek_row_type($arr); else return false;
};
$cmd['e5'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['e5'][CMD_ARRAY]->result_long=CMD_ROW;


$cmd['e4'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $res=$arr->res; else $res=$arr;
};
$cmd['e4'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['e4'][CMD_ARRAY]->result_long=CMD_ROW;


$cmd['one_for_each_owner'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$res=Array();
		$owns=Array();
		global $rlink;
		foreach($arr AS $key=>$row){
			$o=0;
			if(is_object($row) && isset($row->id)){
				if(isset($row->owner)) $o=$row->owner;
			} else if(is_numeric($row)){
				seek_rlink($row);//по идее это потом можно вынести в один запрос, просто тут предполагается что будет производиться работа с уже имеющимися rlink-ами
				if(isset($rlink[$row]->owner)) $o=$rlink[$row]->owner;
			}
			if($op->data=='one_for_each_owner'){
				if($o==0 || !isset($owns[$o])) $res[]=$row;
			} else {
				$res[$o]=$row;
			}
			if(!empty($o)) $owns[$o]=1;
		}
		return $res;
};
$cmd['one_for_each_owner'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['one_for_each_owner'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['one_for_each_owner2'][CMD_ARRAY]->alias=&$cmd['one_for_each_owner'];
$cmd['one_for_each_owner'][CMD_ARRAY]->visual=Array('title'=>'оставить по одному элементу для каждого родителя (оставляет первый элемент)','group'=>GROUP_ARR_OPS);
$cmd['one_for_each_owner2'][CMD_ARRAY]->visual=Array('title'=>'оставить по одному элементу для каждого родителя (оставляет последний элемент)','group'=>GROUP_ARR_OPS);


$cmd['count'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		if(is_array($arr)) $res=count($arr); else $res=0;
		return $res;
};
$cmd['count'][CMD_ARRAY]->result=CMD_STRING;
$cmd['count'][CMD_ARRAY]->visual=Array('title'=>'количество','result'=>STRING_NUM);


$cmd['combine'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$res=0;
		if(is_array($arr)) foreach($arr AS $item){
			if(is_numeric($item)) $res+=$item;
		}
		return $res;
};
$cmd['combine'][CMD_ARRAY]->result=CMD_STRING;
$cmd['combine'][CMD_ARRAY]->visual=Array('title'=>'сложить все элементы массива','group'=>GROUP_ARR_OPS);


$cmd['scount'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		return get_count($arr);
};
$cmd['scount'][CMD_ARRAY]->result=CMD_STRING;
$cmd['scount'][CMD_ARRAY]->visual=Array('title'=>'количество (учитывая дочерние элементы)','group'=>GROUP_ARR_SUB,'result'=>STRING_NUM);


$cmd['last'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		$res=0;
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		if(is_array($arr)){
			foreach($arr AS $v1=>$v2){
				$res=$v1;
			}
			$res=$arr[$res];
		} else $res=0;
		return $res;//prepend_type($res,'row');
};
$cmd['last'][CMD_ARRAY]->result=CMD_MIXED;
$cmd['last'][CMD_ARRAY]->visual=Array('title'=>'последний','group'=>GROUP_ARR_NAV);


$cmd['first'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		$res=0;
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		if(is_array($arr)){
			foreach($arr AS $v1=>$v2){			
				$res=$arr[$v1];break;
			}
		}else $res=0;
		return $res;//prepend_type($res,'row');
};
$cmd['first'][CMD_ARRAY]->result=CMD_MIXED;
$cmd['first'][CMD_ARRAY]->visual=Array('title'=>'первый','group'=>GROUP_ARR_NAV);


$cmd['usel'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		$res=Array();
		if(!is_array($arr) && isset($arr->rows)) $arr=$arr->rows;
		if(!is_array($arr)) return $res;
		global $rlink,$user;
		foreach($arr AS $r){
			if(is_object($r) && isset($r->id)) $o=$r;
			else if(!is_object($r) && is_num($r)){
				seek_rlink($r);
				$o=$rlink[$r];
			}
			if(isset($o->user) && $o->user==$user->id) $res[]=$o;
		}
		return $res;
};
$cmd['usel'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['usel'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['usel'][CMD_ARRAY]->visual=Array('title'=>'элементы, которыми владеет текущий пользователь','group'=>GROUP_ARR_NEW);


$cmd['owners'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		global $rlink;
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$res=Array();$owns=Array();
		if(!empty($arr)) foreach($arr AS $ar){
			if(isset($ar->id)) $owns[$ar->id]=$ar->id;
		}
		if(!empty($owns)){
			$rr=Array();
			$tmp=Array();
			foreach($owns AS $o){
				if(isset($rlink[$o])) $rr[$rlink[$o]->owner]=$rlink[$o]->owner;
				else $tmp[$o]=$o;
			}
			if(!empty($tmp)){
				$res=get_rows_from_array(implode(',',$tmp));
				foreach($res AS $r) $rr[$r["owner_id"]]=$r["owner_id"];
			}
			$res=get_rows_from_array(implode(',',$rr));
			$res=db_to_rows($res);//хотя тут можно было и просто перебрать по rlink, но не суть..
		}
		return $res;
};
$cmd['owners'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['owners'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['owners'][CMD_ARRAY]->visual=Array('title'=>'массив родителей элементов','group'=>GROUP_ARR_NEW);


$cmd['search'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$v[0]=parse_var($op->put[0]);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;		
		if(isset($op->put[1])) $v[1]=parse_var($op->put[1]); else $v[1]='regular';
		if(isset($op->put[2])) $v[2]=parse_var($op->put[2]); else $v[2]='';
		return index_search($arr,$v[0],$v[2],$v[1]);
};
$cmd['search'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['search'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['search'][CMD_ARRAY]->visual=Array('title'=>'поиск по индексируемым полям','put'=>Array(
		0=>Array('title'=>'искомая фраза','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'строгость поиска','type'=>CMD_STRING,'req'=>0,	'list'=>Array('strict'=>'Строгий','regular'=>'Обычный','easy'=>'Облегчённый')),
		2=>Array('title'=>'поля поиска','type'=>STRING_COLNAMES_SEARCH,'req'=>0)
),'group'=>GROUP_ARR_FILTER);


$cmd['prepare'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		global $db;
		if(!is_object($arr) && is_array($arr)) $arr=seek_row_type($arr);
		if(is_object($arr) && !empty($arr->rows)){
			$res=Array();
			$rows_ids=implode(',',collect_rows($arr->rows));
			
			check_single($op->put,$GLOBALS["lex_func_sep"]);
			$optimize_sub=false;
			$tcols=Array();
			$ttables=Array();
			$may_change=true;
			$seek_all=false;
			foreach($op->put AS $vv){
				$vv=parse_var($vv);
				if(strpos($vv,'sub2:')!==false || $vv=='sub2'){
					$vv=str_replace('sub2','sub',$vv);
					$may_change=false;
				}
				if(substr($vv,0,4)=='sub:'){
					$ttables[substr($vv,4)]="'".substr($vv,4)."'";
				} else if($vv=='sub'){
					$optimize_sub=true;
				} else {
					if($vv=='*') $seek_all=true;
					$tcols[$vv]="'".$vv."'";
				}
			}
			if(!empty($tcols) || $seek_all){
				if(!$seek_all){
					$tcols=implode(',',$tcols);
					$cols=getall($db,"SELECT * FROM main_col WHERE col_table=$arr->table AND col_sname IN ($tcols)",1,"main_col");
				} else {
					$cols=getall($db,"SELECT * FROM main_col WHERE col_table=$arr->table",1,"main_col");
					global $rowcol;
					if(!empty($cols)) foreach($cols AS $col){
						$rowcol[$arr->table][$col["col_sname"]]=$col;
					}
				}
				if(!empty($cols)) tree_optimize(0,$arr->rows,$cols);
			}
			if(!empty($ttables)){
				$tbls=getall($db,"SELECT * FROM table_sub WHERE sub_table1=".$arr->table,1,"table_sub");
				$tbls2=Array();
				if(!empty($tbls)){
					foreach($tbls AS $tbl) $tbls2[$tbl["sub_table2"]]=$tbl["sub_table2"];
					$tbls=getall($db,"SELECT * FROM main_table WHERE table_id IN (".implode(',',$tbls2).")",1,"main_table");
					if(!empty($tbls)) foreach($tbls AS $tbl) if(isset($ttables[$tbl["table_sname"]])){
						$res=get_sub($rows_ids,$tbl["table_id"],1,/*0*//*1*/2,0,0,0,$arr->tex,$arr->ex,$table,0,$table,0,$res);
					}
				}
			}
			if($optimize_sub){
				$res=get_sub($rows_ids,$arr->table,1,0,0,0,0,$arr->tex,$arr->ex,$arr->table,0,$arr->table,0,$res);
			}
			if(!empty($cols) && empty($res) || !$may_change) $res=$arr->rows;
		} else $res=Array();
		return $res;
};
$cmd['prepare'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['prepare'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['prepare'][CMD_ARRAY]->visual=Array('title'=>'подгрузка','special'=>SP_PREPARE,'group'=>GROUP_ARR_SPEC);


$cmd['pos'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		$u=parse_var($op->put);
		if(is_object($u) && isset($u->id)) $u=$u->id;
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$i=0;
		foreach($arr AS $var=>$value){
			$i++;
			$w=$value;
			if(is_object($w) && isset($w->id)) $w=$w->id;
			if($w==$u) {$res=$i; break;}
		}
		if(!isset($res)) $res=0;
		return $res;
};
$cmd['pos'][CMD_ARRAY]->result=CMD_STRING;
$cmd['pos'][CMD_ARRAY]->visual=Array('title'=>'Выдать элемент по порядковому номеру',	'put'=>Array(
		0=>Array('title'=>'позиция','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_ARR_NAV);


$cmd['del'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		$v=parse_var($op->put);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$res=Array();
		foreach($arr AS $key=>$ar){
			if($key!=$v) $res[$key]=$ar;
		}
		return $res;
};
$cmd['del'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['del'][CMD_ARRAY]->visual=Array('title'=>'удалить элемент по индексу','put'=>Array(
		0=>Array('title'=>'Индекс элемента','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_ARR_OPS);


$cmd['deldouble'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(empty($op->put)){
			if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
			$res=Array();
			$check=Array();
			$desc=($op->data=='deldouble2');
			if($desc) $arr=array_reverse($arr);
			foreach($arr AS $key=>$value){
				$t=serialize($value);
				if(!isset($check[$t])){
					$res[$key]=$value;
					$check[$t]=1;
				}
			}
			return $res;
		} else {
			if(!is_object($arr) || !isset($arr->rows) || !isset($arr->table)){
				$arr=seek_row_type($arr);
			}
			if($op->data=='deldouble2'){
				$desc=true;
			} else $desc=false;
			$table=$arr->table;
			$arr=$arr->rows;
			$ids=collect_rows($arr);
			if(empty($ids)) return false;
			$res=Array();
			$v=parse_var($op->put);
			global $db;
			$col=getrow($db,"SELECT col_id, col_sname, col_table FROM main_col WHERE col_table=$table AND col_sname='$v'",1,"main_col");
			if(empty($col)) return false;
			$col_id=$col["col_id"];
			$vals=getall6($db,"SELECT value_row, value_col, value_value FROM row_value WHERE value_row IN (".implode(',',$ids).") AND value_col=$col_id","value_row","value_value");
			$check=Array();
			$ignore=Array();
			if($desc) $ids=array_reverse($ids);
			foreach($ids AS $id){
				if(isset($vals[$id]) && !empty($vals[$id])){
					if(isset($check[$vals[$id]])) $ignore[$id]=$id;
					$check[$vals[$id]]=$vals[$id];
				}
			}
			$res=filter_rows_except($arr,$ignore);
			return $res;
		}
};
$cmd['deldouble'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['deldouble'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['deldouble2'][CMD_ARRAY]->alias=&$cmd['deldouble'][CMD_ARRAY];
$cmd['deldouble'][CMD_ARRAY]->visual=Array('title'=>'Удалить повторяющиеся элементы','put'=>Array(
		0=>Array('title'=>'проверочное поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>0)
),'group'=>GROUP_ARR_OPS);
$cmd['deldouble2'][CMD_ARRAY]->visual=Array('title'=>'Удалить повторяющиеся элементы (с конца)','put'=>Array(
		0=>Array('title'=>'проверочное поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>0)
),'group'=>GROUP_ARR_OPS);


$cmd['next'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		$u=parse_var($op->put);
		if(is_object($u) && isset($u->id)) $u=$u->id;
		$res='';
		$back='';
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$tree=get_tmp_tree($arr);
		if(isset($tree[$u])) $res=$tree[$u]->next;
		//if(is_numeric($res)){//is integer тут не катит почему-то
		//	global $rlink;
		//	seek_rlink($res);
		//	if(isset($rlink[$res])) $res=$rlink[$res];
		//}
		if(empty($res)) return false;
		return prepend_type($res,'row');
};
$cmd['next'][CMD_ARRAY]->result=CMD_ROW;
$cmd['next'][CMD_ARRAY]->visual=Array('title'=>'следующий элемент, идущий за выбранным','put'=>Array(
		0=>Array('title'=>'элемент','type'=>CMD_ROW,'req'=>1)
),'group'=>GROUP_ARR_NAV);


$cmd['prev'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		$u=parse_var($op->put);
		if(is_object($u) && isset($u->id)) $u=$u->id;
		$res='';
		$back='';
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$tree=get_tmp_tree($arr);
		if(isset($tree[$u])) $res=$tree[$u]->prev;
		//if(is_numeric($res)){//is integer тут не катит почему-то
		//	global $rlink;
		//	seek_rlink($res);
		//	if(isset($rlink[$res]))	$res=$rlink[$res];
		//}
		if(empty($res)) return false;
		return prepend_type($res,'row');
};
$cmd['prev'][CMD_ARRAY]->result=CMD_ROW;
$cmd['предыдущий элемент'][CMD_ARRAY]->visual=Array('title'=>'предыдущий элемент, идущий перед выбранным','put'=>Array(
		0=>Array('title'=>'элемент','type'=>CMD_ROW,'req'=>1)
),'group'=>GROUP_ARR_NAV);


$cmd['sel'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		return row_filter($arr,'',$op->put,2,1);
};
$cmd['sel'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['sel'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['sel'][CMD_ARRAY]->visual=Array('title'=>'фильтр по условию','put'=>Array(
		0=>Array('title'=>'условия','type'=>CMD_IF,'use_cur'=>1,'req'=>1)
),'group'=>GROUP_ARR_FILTER,'pos'=>1);


$cmd['seld'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		$res=row_filter($arr,'',$op->put,2,0);
		return $res;
};
$cmd['seld'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['seld'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['seld'][CMD_ARRAY]->visual=Array('title'=>'линейный фильтр','put'=>Array(
		0=>Array('title'=>'условия','type'=>CMD_IF,'use_cur'=>1,'req'=>1)
),'group'=>GROUP_ARR_FILTER,'pos'=>5);


$cmd['lsel'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		$res=row_filter($arr,'',$op->put,2,0,1);
		return $res;
};
$cmd['lsel'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['lsel'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['lsel'][CMD_ARRAY]->visual=Array('title'=>'линейный фильтр по дереву','put'=>Array(
		0=>Array('title'=>'условия','type'=>CMD_IF,'use_cur'=>1,'req'=>1)
),'group'=>GROUP_ARR_FILTER,'pos'=>6);


$cmd['asc'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(!empty($op->put)){
			$name=parse_var($op->put);
			$res=row_order($arr,'',$name,'ASC',2);
		} else {
			if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
			if(!is_array($arr)) return false;
			asort($arr);
			$res=$arr;
		}
		return $res;
};
$cmd['asc'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['asc'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['order'][CMD_ARRAY]->alias=&$cmd['asc'][CMD_ARRAY];
$cmd['asc'][CMD_ARRAY]->visual=Array('title'=>'сортировка','put'=>Array(
		0=>Array('title'=>'поля сортировки','type'=>STRING_COLNAMES_ORDER,'in_quotes'=>1,'req'=>1)
),'group'=>GROUP_ARR_SORT);


$cmd['desc'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(!empty($op->put)){
			$name=parse_var($op->put);
			$res=row_order($arr,'',$name,'DESC',2);
		} else {
			if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
			if(!is_array($arr)) return false;
			arsort($arr);
			$res=$arr;
		}
		return $res;
};
$cmd['desc'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['desc'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['desc'][CMD_ARRAY]->visual=Array('title'=>'обратная сортировка','put'=>Array(
		0=>Array('title'=>'поля сортировки','type'=>STRING_COLNAMES_ORDER,'in_quotes'=>1,'req'=>1)
),'group'=>GROUP_ARR_SORT);


$cmd['fastget'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		return fastget($op,$ops,$arr);
};
$cmd['fastget'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['fastget'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['fastget2'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['fastget3'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['ifastget'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['ifastget2'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['ifastget3'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];

$cmd['efastget'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['efastget2'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['efastget3'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['eifastget'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['eifastget2'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['eifastget3'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];

$cmd['afastget'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['afastget2'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['afastget3'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['aifastget'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['aifastget2'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];
$cmd['aifastget3'][CMD_ARRAY]->alias=&$cmd['fastget'][CMD_ARRAY];


$cmd['fastgetc'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		return fastget($op,$ops,$arr);
};
$cmd['fastgetc'][CMD_ARRAY]->result=CMD_STRING;
$cmd['fastget2c'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['fastget3c'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['ifastgetc'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['ifastget2c'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['ifastget3c'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];

$cmd['efastgetc'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['efastget2c'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['efastget3c'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['eifastgetc'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['eifastget2c'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['eifastget3c'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];

$cmd['afastgetc'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['afastget2c'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['afastget3c'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['aifastgetc'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['aifastget2c'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];
$cmd['aifastget3c'][CMD_ARRAY]->alias=&$cmd['fastgetc'][CMD_ARRAY];

$cmd['fastget'][CMD_ARRAY]->visual=Array('title'=>'быстрая выборка','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>0),
		1=>Array('title'=>'условие','type'=>CMD_FASTGET_IF,'req'=>1),
		2=>Array('title'=>'сортировка','type'=>CMD_FASTGET_ORDER,'req'=>0),
		3=>Array('title'=>'начальный элемент','type'=>CMD_STRING,'req'=>0),
		4=>Array('title'=>'количество','type'=>CMD_STRING,'req'=>0)
),'special'=>SP_FASTGET,'group'=>GROUP_ARR_FILTER,'hidden'=>1);
$cmd['fastgetc'][CMD_ARRAY]->visual=Array('title'=>'быстрая выборка (количество)','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>0),
		1=>Array('title'=>'условие','type'=>CMD_FASTGET_IF,'req'=>1),
		2=>Array('title'=>'сортировка','type'=>CMD_FASTGET_ORDER,'req'=>0),
		3=>Array('title'=>'начальный элемент','type'=>CMD_STRING,'req'=>0),
		4=>Array('title'=>'количество','type'=>CMD_STRING,'req'=>0)
),'special'=>SP_FASTGET,'group'=>GROUP_ARR_FILTER,'result'=>STRING_NUM,'hidden'=>1);


$cmd['find'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		return fastget($op,$ops,$arr);
};
$cmd['find'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['find'][CMD_ARRAY]->visual=Array('title'=>'Выбрать объекты','put'=>Array(
		0=>Array('title'=>'Игнорировать родителя','type'=>CMD_LOGICAL,'static'=>'ignore_owner','req'=>0),
		1=>Array('title'=>'Проверять доступы','type'=>CMD_LOGICAL,'static'=>'check_auth','req'=>0),
		2=>Array('title'=>'Подтаблица','type'=>STRING_SUBTABLE_NAMES,'sname'=>'subtable','req'=>0),
		3=>Array('title'=>'Условия поиска','type'=>CMD_STRING,'list'=>Array(''=>'Нет','ignore_ex'=>'Игнорировать экземпляр таблицы','ignore_module'=>'Игнорировать модуль'),'req'=>0),
		4=>Array('title'=>'Какие объекты возвращать','type'=>CMD_STRING,'list'=>Array(''=>'Активные','disabled'=>'Не активные','all'=>'Все'),'req'=>0),
		5=>Array('title'=>'Дополнительные условия','type'=>CMD_STRING,'list'=>Array(''=>'Нет','return_owners'=>'Вернуть родителей найденных объектов'),'req'=>0)
),'group'=>GROUP_ARR_FILTER,'pos'=>0,'result'=>CMD_FIND);


$cmd['find_count'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		return fastget($op,$ops,$arr);
};
$cmd['find_count'][CMD_ARRAY]->result=CMD_STRING;//STRING_NUM
$cmd['find_count'][CMD_ARRAY]->visual=Array('title'=>'Выдать количество объектов','put'=>Array(
		0=>Array('title'=>'Игнорировать родителя','type'=>CMD_LOGICAL,'static'=>'ignore_owner','req'=>0),
		1=>Array('title'=>'Подтаблица','type'=>STRING_SUBTABLE_NAMES,'sname'=>'subtable','req'=>0),
		2=>Array('title'=>'Условия поиска','type'=>CMD_STRING,'list'=>Array(''=>'Нет','ignore_ex'=>'Игнорировать экземпляр таблицы','ignore_module'=>'Игнорировать модуль'),'req'=>0),
		3=>Array('title'=>'Какие объекты возвращать','type'=>CMD_STRING,'list'=>Array(''=>'Активные','disabled'=>'Не активные','all'=>'Все'),'req'=>0),
		4=>Array('title'=>'Дополнительные условия','type'=>CMD_STRING,'list'=>Array(''=>'Нет','return_owners'=>'Вернуть родителей найденных объектов'),'req'=>0)
),'group'=>GROUP_ARR_FILTER,'pos'=>0,'result'=>CMD_FIND);


$cmd['if'][CMD_ARRAY]->process=function(&$str,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		if(isset($op->put[0]) && !empty($arr)) return parse_var($op->put[0]);
		if(isset($op->put[1]) && empty($arr)) return parse_var($op->put[1]);
		return $str;
};
$cmd['if'][CMD_ARRAY]->result=CMD_MIXED;
$cmd['if'][CMD_ARRAY]->visual=Array('title'=>'условие','work_on'=>CMD_LOGICAL,'put'=>Array(
		0=>Array('title'=>'Действие на выполнение условия','type'=>CMD_CMD,'req'=>1),
		1=>Array('title'=>'Иначе','type'=>CMD_CMD,'req'=>0)
),'group'=>GROUP_ARR_SPEC);


$cmd['with'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		$res=backup_globals();
		$GLOBALS['for_value']=$arr;
		if(!isset($GLOBALS["spec_step"])) $GLOBALS["spec_step"]=0;
		if(!isset($GLOBALS["spec_step2"])) $GLOBALS["spec_step2"]=0;
		$GLOBALS["spec_step"]++;
		$GLOBALS["spec_step2"]++;			
		$r=parse_var($op->put);
		return_globals($res);
		return $r;
};
$cmd['with'][CMD_ARRAY]->result=CMD_MIXED;
$cmd['with'][CMD_ARRAY]->visual=Array('title'=>'Исполнить оператор','put'=>Array(
		0=>Array('title'=>'оператор','type'=>CMD_CMD,'req'=>1)
),'group'=>GROUP_ARR_SPEC);


$cmd['del_all'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		global $rlink;
		foreach($arr AS $var=>$row){
			if(is_object($row) && isset($row->id)) $row=$row->id;
			seek_rlink($row);
			$ex=get_ex($row,$rlink[$row]->table);
			if(!check_row($row,$rlink[$row]->table,$ex,'del',$rlink[$row]->user,$rlink[$row]->users)) unlink($arr[$var]);
		}
		if(!empty($arr)) del_row($arr);
};
$cmd['del_all'][CMD_ARRAY]->result=CMD_NONE;
$cmd['del_all'][CMD_ARRAY]->visual=Array('title'=>'Удалить массив объектов','group'=>GROUP_ARR_OPS);


$cmd['walk'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$i=0;
		if(!is_array($arr)) return false;
		$res=backup_globals();	
		$GLOBALS["count"]=count($arr);
		if(!isset($GLOBALS["spec_step"])) $GLOBALS["spec_step"]=0;
		if(!isset($GLOBALS["spec_step2"])) $GLOBALS["spec_step2"]=0;
		$GLOBALS["spec_step"]++;
		$GLOBALS["spec_step2"]++;
		$collect_array=($op->data=='map');
		$totalS='';
		$totalA=Array();
		foreach($arr AS $var=>$tre){
			$GLOBALS["break"]=0;
			if(is_object($tre)){
				if(empty($GLOBALS["cur_type"])) $GLOBALS["cur_type"]='';
				if(!empty($tre->table)) $GLOBALS["cur_table"]=$tre->table;			
				$GLOBALS["current"]=$tre->id;
				if(!empty($tre->type)){
					$GLOBALS["cur_type"]=$tre->type;
				} else $GLOBALS["cur_type"]='row';
				$GLOBALS["up"][$GLOBALS["spec_step"]]=$tre->id;
				if(!empty($tre->type)) $GLOBALS["up2"][$GLOBALS["spec_step"]]=$tre->type;
				else {
					if(isset($tre->id)) $GLOBALS["up2"][$GLOBALS["spec_step"]]='row';
					else $GLOBALS["up2"][$GLOBALS["spec_step"]]='';
				}
			}
			$GLOBALS["up3"][$GLOBALS["spec_step"]]=$var;
			$GLOBALS["up4"][$GLOBALS["spec_step"]]=$i;
			$GLOBALS["up5"][$GLOBALS["spec_step"]]=$tre;		
			$GLOBALS["index"]=$i;
			$GLOBALS["cur_var"]=$var;
			$GLOBALS["for_value"]=$tre;
			$GLOBALS["last"]=0;$slast=0;
			if($i==$GLOBALS["count"]-1) $GLOBALS["last"]=1;
			
			$tmp=parse_var($op->put);
			if($collect_array && $tmp!==false){
				$totalA[]=$tmp;
			} else {
				if(is_object($tmp) && isset($tmp->id)) $tmp=$tmp->id;
				if(!is_object($tmp) && !is_array($tmp)) $totalS.=$tmp;
			}
			
			if(!empty($GLOBALS["break"])){ $GLOBALS["break"]=false; break; }		
			$i++;
		}
		return_globals($res);
		if($collect_array) return $totalA; else return $totalS;
};
$cmd['walk'][CMD_ARRAY]->result=CMD_STRING;//CMD_MIXED;//CMD_NONE;
$cmd['tree'][CMD_ARRAY]->alias=&$cmd['walk'][CMD_ARRAY];
$cmd['each'][CMD_ARRAY]->alias=&$cmd['walk'][CMD_ARRAY];
$cmd['walk'][CMD_ARRAY]->visual=Array('title'=>'применение оператора ко всем элементам массива','put'=>Array(
		0=>Array('title'=>'оператор','type'=>CMD_CMD,'use_cur'=>1,'req'=>1)
),'group'=>GROUP_ARR_SPEC);
$cmd['map'][CMD_ARRAY]->alias=&$cmd['walk'][CMD_ARRAY];
$cmd['map'][CMD_ARRAY]->result=CMD_ARRAY;//CMD_MIXED;//CMD_NONE;
$cmd['map'][CMD_ARRAY]->visual=Array('title'=>'составить новый массив, применяя указанный оператор','put'=>Array(
		0=>Array('title'=>'оператор','type'=>CMD_CMD,'use_cur'=>1,'req'=>1)
),'group'=>GROUP_ARR_SPEC);


$cmd['get'][CMD_ARRAY]->process=function(&$data,&$op,&$ops,$from_table=0,$table=0,$tex=0,$owner=0){
		if(!$from_table){
			$arr=&$data; $table=find_table_in_arr($data);	
			if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
			$rows_ids=implode(',',collect_rows($arr));
			if(empty($rows_ids)){
				if($op->data=='get') return false;
				if($op->data=='gets') return Array();
			}
		}
		global $db;
		$SQL_add1='';
		$SQL_add2='';
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		if(isset($op->put[1])){
			$v=safe_sql_input(parse_var($op->put[1]));
			$SQL_add1=' LEFT JOIN main_col AS c ON v.value_col=c.col_id';
			$col=parse_var($op->put[0]);
			if(is_object($col) && isset($col->id)){
				$col_id=safe_sql_input($col->id);
				$SQL_add2=" AND c.col_id=$col_id";
			} else if(is_numeric($col)){
				$col_id=safe_sql_input($col);
				$SQL_add2=" AND c.col_id=$col_id";
			} else {
				$col_sname=safe_sql_input($col);
				$SQL_add2=" AND c.col_sname='$col_sname'";
			}
		} else {
			$v=safe_sql_input(parse_var($op->put[0]));
		}
		if(!$from_table) $rows=getall3($db,"SELECT v.value_row FROM row_value AS v".$SQL_add1." WHERE v.value_row IN ($rows_ids) AND v.value_value='$v'".$SQL_add2.' GROUP BY v.value_row','value_row');
		else $rows=getall($db,"SELECT r.ro_enable, r.owner_id, r.owner_table, r.ro_id, r.ro_ex, r.ro_pos, r.ro_sub, r.row_id, r.row_table, r.ro_user, r.ro_users, r.row_module FROM row_owner AS r LEFT JOIN row_value AS v ON v.value_row=r.row_id".$SQL_add1." WHERE r.ro_ex=$tex AND r.row_table=$table AND r.ro_sub=$owner".$SQL_add2." AND v.value_value='$v' GROUP BY v.value_row");
		if($op->data=='gets'){
			if(!$from_table) return filter_rows($arr,$rows);
			else return rows_to_rlink2($rows);
		} else {
			if(!empty($rows)){
				foreach($rows AS $row){
					global $rlink;
					if(is_array($row)) $row=$row['row_id'];
					seek_rlink($row);
					return $rlink[$row];
				}
			} else return false;
		}
};
$cmd['get'][CMD_ARRAY]->result=CMD_ROW;
$cmd['get'][CMD_ARRAY]->visual=Array('title'=>'выдать элемент по значению поля','put'=>Array(
		0=>Array('title'=>'поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>0),
		1=>Array('title'=>'значение','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_ARR_FILTER,'pos'=>2);


$cmd['gets'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$x=get_cmd_link('get',CMD_ARRAY);
		return $x($data,$op,$ops);
};
$cmd['gets'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['gets'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['gets'][CMD_ARRAY]->visual=Array('title'=>'выдать элементы по значению поля','put'=>Array(
		0=>Array('title'=>'поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>0),
		1=>Array('title'=>'значение','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_ARR_FILTER,'pos'=>3);


$cmd['rand'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		srand((float) microtime() * 10000000);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		if(!is_array($arr)) return false;
		if(empty($op->put)){
			shuffle($arr);
			return $arr;
		} else {
			$x=parse_var($op->put);
			$res=Array();
			if($x>count($arr)) $x=count($arr);
			if(count($arr)>1){
				if($x==1){
					$v=array_rand($arr, 2);
					$res2=Array();
					if(is_array($v))	foreach($v AS $vv) $res2[]=$arr[$vv];
					else $res2[]=$v;
					$res[]=$res2[0];
				} else if(empty($x) || !is_numeric($x) || $x<0){
				} else  {
					if($x>count($arr)) $x=count($arr);					
					$v=array_rand($arr, $x);
					if(is_array($v))	foreach($v AS $vv) $res[]=$arr[$vv];
					else $res[]=$v;
				}
			} else $res=$arr;
		}
		return $res;
};
$cmd['rand'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['rand'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['rand'][CMD_ARRAY]->visual=Array('title'=>'случайная сортировка','put'=>Array(
		0=>Array('title'=>'количество элементов в результате','type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_ARR_SORT);


$cmd['kasc'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		if(is_array($arr)){
			ksort($arr);
			return $arr;
		}else return $arr;
};
$cmd['kasc'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['kasc'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['kasc'][CMD_ARRAY]->visual=Array('title'=>'сортировка по ключам','group'=>GROUP_ARR_SORT);


$cmd['kdesc'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		if(is_array($arr)){
			krsort($arr);
			return $arr;
		}else return $arr;
};
$cmd['kdesc'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['kdesc'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['kdesc'][CMD_ARRAY]->visual=Array('title'=>'обратная сортировка по ключам','group'=>GROUP_ARR_SORT);


$cmd['flip'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		if(is_array($arr)){
			return array_flip($arr);
		}else return $arr;
};
$cmd['flip'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['flip'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['flip'][CMD_ARRAY]->visual=Array('title'=>'поменять местами ключи и значения','group'=>GROUP_ARR_OPS);


$cmd['sort'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		
		if(!empty($op->put)){
			check_single($op->put,$GLOBALS["lex_func_sep"]);
			$condition=Array();
			if(!empty($op->put[0]->next_operation) && $op->put[0]->next_operation==',') {
				foreach($op->put AS $o){
					$condition[]=$o;
				}
			} else $condition=Array($op->put);
			
			$i=0;
			$res=backup_globals();	
			$GLOBALS["count"]=count($arr);
			if(!isset($GLOBALS["spec_step"])) $GLOBALS["spec_step"]=0;
			if(!isset($GLOBALS["spec_step2"])) $GLOBALS["spec_step2"]=0;
			$GLOBALS["spec_step"]++;
			$GLOBALS["spec_step2"]++;
			$dest=Array();
			foreach($arr AS $var=>$tre){
				$GLOBALS["break"]=0;
				if(is_object($tre)){
					if(empty($GLOBALS["cur_type"])) $GLOBALS["cur_type"]='';
					if(!empty($tre->table)) $GLOBALS["cur_table"]=$tre->table;			
					$GLOBALS["current"]=$tre->id;
					if(!empty($tre->type)){
						$GLOBALS["cur_type"]=$tre->type;
					} else $GLOBALS["cur_type"]='row';
					$GLOBALS["up"][$GLOBALS["spec_step"]]=$tre->id;
					if(!empty($tre->type)) $GLOBALS["up2"][$GLOBALS["spec_step"]]=$tre->type;
					else {
						if(isset($tre->id)) $GLOBALS["up2"][$GLOBALS["spec_step"]]='row';
						else $GLOBALS["up2"][$GLOBALS["spec_step"]]='';
					}
				}
				$GLOBALS["up3"][$GLOBALS["spec_step"]]=$var;
				$GLOBALS["up4"][$GLOBALS["spec_step"]]=$i;
				$GLOBALS["up5"][$GLOBALS["spec_step"]]=$tre;		
				$GLOBALS["index"]=$i;
				$GLOBALS["cur_var"]=$var;
				$GLOBALS["for_value"]=$tre;
				$GLOBALS["last"]=0;$slast=0;
				if($i==$GLOBALS["count"]-1) $GLOBALS["last"]=1;

				$s='';
				foreach($condition AS $c){
					if(is_array($c) && count($c)==1) $c=$c[0];
					$tmp=parse_var($c);
					if(is_object($tmp) && isset($tmp->id)) $tmp=$tmp->id;
					if(is_array($tmp)) $tmp='';
					if(empty($tmp)) $tmp='0';
					if(!empty($s)) $s.=',';
					$s.=$tmp;
				}

				if(is_object($tre) && isset($tre->id)){
					$a=$tre->id;
				} else {
					$a=1;
					while(isset($dest[$s][$a])) $a++;
				}
				$dest[$s][$a]=$tre;
				
				if(!empty($GLOBALS["break"])){ $GLOBALS["break"]=false; break; }
				$i++;
			}
			uksort($dest, "strnatcmp");
			$arr=Array();
			foreach($dest AS $tmp) foreach($tmp AS $a) $arr[]=$a;
			//arsort($dest);
			return_globals($res);
			return $arr;
			
		} else return $arr;
		
};
$cmd['sort'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['sort'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['sort'][CMD_ARRAY]->visual=Array('title'=>'Сортировать по выражению','put'=>Array(
		0=>Array('title'=>'выражение','type'=>CMD_CMD,'is_array'=>1,'ignore_array_in_combine'=>1,'req'=>1,'use_cur'=>1)
),'group'=>GROUP_ARR_SORT);


$cmd['json_encode'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){		
		return json_encode($data);
};
$cmd['json_encode'][CMD_ARRAY]->result=CMD_STRING;
$cmd['json_encode'][CMD_ARRAY]->visual=Array('title'=>'Кодировать в JSON','group'=>GROUP_ARR_SPEC);


$cmd['serialize'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){	
		return serialize($data);
};
$cmd['serialize'][CMD_STRING]->result=CMD_STRING;
$cmd['serialize'][CMD_STRING]->visual=Array('title'=>'Кодировать в Serialize','group'=>GROUP_ARR_SPEC);


$cmd['rows'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$res=Array();
		if(is_array($arr) && !empty($arr)) foreach($arr AS $ar) if($ar->enable) $res[]=$ar;	
		return $res;
};
$cmd['rows'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['rows'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['rows'][CMD_ARRAY]->visual=Array('title'=>'активные объекты','hidden'=>1);


$cmd['rows_all'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$res=Array();
		if(is_array($arr) && !empty($arr)) foreach($arr AS $ar) $res[]=$ar;	
		return $res;
};
$cmd['rows_all'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['rows_all'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['rows2'][CMD_ARRAY]->alias=&$cmd['rows_all'][CMD_ARRAY];
$cmd['rows_all'][CMD_ARRAY]->visual=Array('title'=>'все объекты','hidden'=>1);


$cmd['rows_na'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$res=Array();
		if(is_array($arr) && !empty($arr)) foreach($arr AS $ar) if(!$ar->enable) $res[]=$ar;	
		return $res;
};
$cmd['rows_na'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['rows_na'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['rows3'][CMD_ARRAY]->alias=&$cmd['rows_na'][CMD_ARRAY];
$cmd['rows_na'][CMD_ARRAY]->visual=Array('title'=>'деактивированные объекты','hidden'=>1);


$cmd['rows_auth'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$res=Array();
		if(is_array($arr) && !empty($arr)) foreach($arr AS $ar) if($ar->enable && check_row($ar->id,$ar->table,get_ex($ar->id,$ar->table,$ar->tex),'view',$ar->user,$ar->users,$ar->module)!=0) $res[]=$ar;
		return $res;
};
$cmd['rows_auth'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['rows_auth'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['arows'][CMD_ARRAY]->alias=&$cmd['rows_auth'][CMD_ARRAY];
$cmd['rows_auth'][CMD_ARRAY]->visual=Array('title'=>'доступные пользователю объекты','hidden'=>1);


$cmd['limit'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		$x=parse_var($op->put[0]);
		if(isset($op->put[1])) $y=parse_var($op->put[1]); else {
			$y=$x;
			$x=0;
		}
		$res=Array();$i=0;$i2=0;
		if(!empty($arr)) foreach($arr AS $v2=>$value){
			$i2++;
			if($i2>$x){
				$i++;
				if($i>$y) break;
				$res[$v2]=$value;
			}
		}
		return $res;
};
$cmd['limit'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['limit'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['limit'][CMD_ARRAY]->visual=Array('title'=>'обрезка','put'=>Array(
		0=>Array('title'=>'начало','type'=>CMD_STRING,'req'=>0),
		0=>Array('title'=>'количество','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_ARR_OPS);


$cmd['implode'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		return implode(parse_var($op->put),$arr);
};
$cmd['implode'][CMD_ARRAY]->result=CMD_STRING;
$cmd['imp'][CMD_ARRAY]->alias=&$cmd['implode'][CMD_ARRAY];
$cmd['implode'][CMD_ARRAY]->visual=Array('title'=>'объеденить элементы','put'=>Array(
		0=>Array('title'=>'разделитель','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_ARR_OPS);


$cmd['sc'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		$itbl=false;
		if($op->data=='sc2' || $op->data=='sc_mixed') $itbl=true;
		if(!empty($op->put)){
			$col=parse_var($op->put);
		} else {
			$op=array_shiftA($ops);
			$col=$op->data;
		}
		global $db;
		if(is_object($arr) && isset($arr->rows)) $arr=$arr->rows;
		if(empty($arr) || !is_array($arr)) return false;
		$clst=Array();
		foreach($arr AS $a){		
			if(empty($a->module)){
				if(is_numeric($a)) $id=$a; else if(is_object($a) && !empty($a->id)) $id=$a->id; else $id=0;
				if($id>0){
					global $rlink;
					seek_rlink($id);
					$table=$rlink[$id]->table;
					$clst[$table]=$table;
					$module=$rlink[$id]->module;
				}
			} else {
				$table=$a->table; $module=$a->module;
				$clst[$table]=$table;
			}
			if(!$itbl) break;
		}
		if(!empty($module)){
			global $pccol2,$pccol;
			if(empty($pccol2[$module]) || empty($pccol2[$module][$table]) || empty($pccol2[$module][$table][$col])){
				//getrow($db,"SELECT * FROM main_col WHERE col_module=$module AND col_table=$table AND col_sname='$col'",1,"main_col");
				getrow($db,"SELECT * FROM main_col WHERE col_table=$table AND col_sname='$col'",1,"main_col");
				$pccol2[$module][$table][$col]=$db->Record;
				if(!empty($db->Record)) $pccol[$db->Record["col_id"]]=$db->Record;
			} else if(!empty($pccol2[$module]) && !empty($pccol2[$module][$table])) $db->Record=$pccol2[$module][$table][$col];
			else {etf('cmd_arr'); return '';}
			if(empty($db->Record)){etf('cmd_arr'); return '';}
			$col_id=$db->Record["col_id"];
			if(empty($col_id)){etf('cmd_arr'); return '';}
			$col_type=$db->Record["col_type"];
			$col_link=$db->Record["col_link"];
			$col_link2=$db->Record["col_link2"];
			$ccol=$db->Record;
			$rws=Array();
			foreach($arr AS $a) $rws[$a->id]=$a->id;
			$rws=implode(',',$rws);
			if(!$itbl || count($clst)<2){
				global $xrows_cache;
				$line='2'.$rws.'.'.$col_id;
				if(!isset($xrows_cache[$line])){
					$vals=getall($db,"SELECT * FROM row_value WHERE value_row IN ($rws) AND value_col=$col_id",1,'row_value',__LINE__,__FILE__);
					$xrows_cache[$line]=$vals;
				} else $vals=$xrows_cache[$line];
			} else {
				$clsc=getall3($db,"SELECT * FROM main_col WHERE col_table IN (".implode(',',$clst).") AND col_sname='$col'","col_id");
				$vals=getall($db,"SELECT * FROM row_value WHERE value_row IN ($rws) AND value_col IN (".implode(',',$clsc).")",1,'row_value');
			}
			if(empty($vals)) $res=Array();
			else {
				$res=Array();$i=0;				
				if(!empty($ccol["col_force_onshow"])){
					foreach($vals AS $index=>$tmp){
						$vals[$index]["value_value"]=force_value($tmp,$ccol);
					}
				}				
				if($col_type==0 || $col_type==2 || $col_type==3){
					foreach($vals AS $va){$res[$i]=$va["value_value"]; $i++;}
				}
				if($col_type==4){			
					foreach($vals AS $va){
						getrow($db,"SELECT * FROM main_module WHERE module_id=".$va["value_value"],1,"main_module");
						if(!empty($db->Record)){
							$res[$i]->id=$va["value_value"];
							$res[$i]->type='md';
							$res[$i]->name=$db->Record["module_name"];
							$res[$i]->sname=$db->Record["module_sname"];
							$i++;
						}
					}
				}
				if($col_type==5){
					foreach($vals AS $va){
						//getrow($db,"SELECT * FROM main_auth WHERE auth_type=1 AND auth_id=".$va["value_value"],1,"main_auth");
						//if(!empty($db->Record)){
							$res[$i]->id=$va["value_value"];
							$res[$i]->type='usr';
							//$res[$i]->name=$db->Record["user_name"];
							//$res[$i]->login=$db->Record["user_login"];
							//$res[$i]->email=$db->Record["user_email"];
							$i++;
						//}
					}
				}
				if($col_type==1){
					$rws=Array();
					global $rlink;
					foreach($vals AS $va) if(is_numeric($va["value_value"])) $rws[$va["value_id"]]=$va["value_value"];
					$rws=implode(',',$rws);
					if(empty($rws)){etf('cmd_arr'); return '';}
					$rws=get_rows_from_array($rws);
					rows_to_rlink($rws);
					foreach($vals AS $va) if(isset($rlink[$va["value_value"]])){
						$res[$i]=$rlink[$va["value_value"]];
						$i++;
					}
					$res=seek_row_type($res);
					//col_link2 проверку не проводим, т.к. безразницы
				}
			}
		}
		return $res;
};
$cmd['sc'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['sc'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['sc2'][CMD_ARRAY]->alias=&$cmd['sc'][CMD_ARRAY];
$cmd['sc_mixed'][CMD_ARRAY]->alias=&$cmd['sc'][CMD_ARRAY];
$cmd['sc'][CMD_ARRAY]->visual=Array('title'=>'массив значений полей','put'=>Array(
		0=>Array('title'=>'поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>1)
)/*,'special'=>SP_POINT*/,'group'=>GROUP_ARR_NEW);
$cmd['sc_mixed'][CMD_ARRAY]->visual=Array('title'=>'массив значений полей (из разных таблиц)','put'=>Array(
		0=>Array('title'=>'поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>1)
)/*,'special'=>SP_POINT*/,'group'=>GROUP_ARR_NEW);


$cmd['st'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		$seek_auth=0; $seek_ena=1; 
		if ($op->data=='st2' || $op->data=='st_all'){
			$seek_auth=0;
			$seek_ena=0;
		}
		if ($op->data=='st3' || $op->data=='st_na'){
			$seek_auth=0;
			$seek_ena=2;
		}
		if ($op->data=='ast' || $op->data=='st_auth'){
			$seek_auth=1;
			$seek_ena=1;
		}
		if(is_object($arr) && isset($arr->rows)) $rows=$arr->rows; else $rows=$arr;
		$op=array_shiftA($ops);
		$table_sname=$op->data;
		
		if(count($rows)>0){	
			global $db;
			$row=get_row_from_rows($rows,true);
			$ex1=$row->tex;
			$ex=get_ex2($ex1);
			$table=$row->table;
			if(!empty($table)){
				$owns=$rows;
				$GLOBALS["ex_ex2"]=$ex;
				//$utpl["rows"][$table]=1;
				$gs->rows=Array(); $gs->ex=$ex;
				if($ex==0) $ex=get_ex2($ex1); if($ex==0) $seek_ex=0; else $seek_ex=1;
				$ignore_table=0;
				//определение подтаблицы
				$stb=getall($db,"SELECT * FROM table_sub WHERE sub_table1=$table",1,"table_sub");
				$xtb='';
				foreach($stb AS $tb){if(!empty($xtb)) $xtb.=','; $xtb.=$tb["sub_table2"];}
				if(empty($xtb)) return '';
				getrow($db,"SELECT * FROM main_table WHERE table_id IN ($xtb) AND table_sname='$table_sname'",1,"main_table");
				if(empty($db->Record["table_id"])) return '';
				$xtbl=$db->Record["table_id"];
				$GLOBALS["utpl"]["rows"][$xtbl.'.'.$ex1]=1;
				$rows=Array();
				//сканирование подтаблиц
				$res=rescan_st($owns,$xtbl,$seek_ena,$seek_ex,$ignore_table,$ex1,$ex,$table,$seek_auth,$rows);
			} else $res=Array();
		} else $res=Array();
		return $res;
};
$cmd['st'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['st'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['st2'][CMD_ARRAY]->alias=&$cmd['st'][CMD_ARRAY];
$cmd['st_all'][CMD_ARRAY]->alias=&$cmd['st'][CMD_ARRAY];
$cmd['st3'][CMD_ARRAY]->alias=&$cmd['st'][CMD_ARRAY];
$cmd['st_na'][CMD_ARRAY]->alias=&$cmd['st'][CMD_ARRAY];
$cmd['ast'][CMD_ARRAY]->alias=&$cmd['st'][CMD_ARRAY];
$cmd['st_auth'][CMD_ARRAY]->alias=&$cmd['st'][CMD_ARRAY];
$cmd['st'][CMD_ARRAY]->visual=Array('title'=>'активные элементы подтаблицы','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_ARR_SUB);
$cmd['st_all'][CMD_ARRAY]->visual=Array('title'=>'все элементы подтаблицы','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_ARR_SUB);
$cmd['st_na'][CMD_ARRAY]->visual=Array('title'=>'не активные элементы подтаблицы','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_ARR_SUB);
$cmd['st_auth'][CMD_ARRAY]->visual=Array('title'=>'элементы подтаблицы, доступные для пользователя','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_ARR_SUB);


$cmd['st_simple'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		global $rlink,$db;
		$op=array_shiftA($ops);
		$table_sname=$op->data;
		if(is_object($arr) && isset($arr->rows)) $rows=$arr->rows; else $rows=$arr;
		if(!empty($rows)){	
			foreach($rows AS $tmp){
				$x=$tmp;
				if(!is_object($x)){				
					seek_rlink($x);
					$x=$rlink[$x];
					$rws=implode(',',collect_rows2($rows));
				} else $rws=implode(',',collect_rows($rows));
				$up_table=$x->table;
				break;
			}
			$stbls=getall3($db,"SELECT * FROM table_sub WHERE sub_table1=$up_table","sub_table2");
			if(!empty($stbls)){
				$table_id=getrowval("SELECT table_sname, table_id FROM main_table WHERE table_sname='$table_sname' AND table_id IN (".implode(',',$stbls).")",'table_id');
				if(!empty($table_id)){
					$res=getall3($db,"SELECT * FROM row_owner WHERE owner_id IN ($rws) AND row_table=$table_id AND ro_enable=1","row_id");
				} else $res=Array();
			} else $res=Array();
		} else $res=Array();
		return $res;
};
$cmd['st_simple'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['st_simple'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['st_simple'][CMD_ARRAY]->visual=Array('title'=>'элементы подтаблицы (быстрый доступ)','put'=>Array(
		0=>Array('title'=>'поле',STRING_SUBTABLE_NAMES,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_ARR_SUB);


$cmd['allcol'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $rows=$arr->rows; else $rows=$arr;
		$res=Array();
		if(!empty($rows)){
			$row=get_row_from_rows($rows);
			$table=$row->table;
			$ex=get_ex2($row->tex);
			$owner=$row->owner;
			$res=allcol($op,$table,$ex,$owner,$rows);
			//$res=allcol($op,$table,0,0,$rows);
		}
		return $res;
};
$cmd['allcol'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['allcol'][CMD_ARRAY]->result_long=CMD_COL;
$cmd['allcol2'][CMD_ARRAY]->alias=&$cmd['allcol'][CMD_ARRAY];
$cmd['allcol_uniq'][CMD_ARRAY]->alias=&$cmd['allcol'][CMD_ARRAY];
/*$cmd['allcol'][CMD_ARRAY]->visual=Array('title'=>'массив значений полей','put'=>Array(
		0=>Array('title'=>'поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>1)
),'group'=>GROUP_ARR_NEW);
$cmd['allcol_uniq'][CMD_ARRAY]->visual=Array('title'=>'массив значений полей (исключить дубликаты)','put'=>Array(
		0=>Array('title'=>'поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>1)
),'group'=>GROUP_ARR_NEW);*/


$cmd['stasc'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$arr=&$data; $table=find_table_in_arr($data);
		if(is_object($arr) && isset($arr->rows)) $rows=$arr->rows; else $rows=$arr;
		if(count($rows)>0){
			$row=get_row_from_rows($rows,true);
			$rows_ids=implode(',',collect_rows($rows));
			$ex1=$row->tex;
			
			$table=$row->table;
			$module=$row->module;
			if(!empty($table) && !empty($module) && !empty($rows_ids)){
				global $db;
				if($op->data=='stasc') $type='';
				if($op->data=='stdesc') $type=' DESC';
				$op=array_shiftA($ops);
				check_single($op->put,$GLOBALS["lex_func_sep"]);
				$table_sname=$op->data;
				$table_id=getrowval("SELECT table_sname, table_id, table_module FROM main_table WHERE table_module=$module AND table_sname='$table_sname'","table_id");
				$col_sname=parse_var($op->put[0]);
				$col_id=getrowval("SELECT col_sname, col_id, col_table FROM main_col WHERE col_table=$table_id AND col_sname='$col_sname'","col_id");
				if(count($op->put)==2){
					$start=0;
					$end=parse_var($op->put[1]);
					$limit=' LIMIT '.$start.','.$end;
				} else if(count($op->put)==3){
					$start=parse_var($op->put[1]);
					$end=parse_var($op->put[2]);
					$limit=' LIMIT '.$start.','.$end;
				} else {
					$start=0;
					$end=0;
					$limit='';
				}
				$query='SELECT ro.*, rw.value_value, rw.value_row, rw.value_col FROM row_owner AS ro LEFT JOIN row_value AS rw ON ro.row_id=rw.value_row AND rw.value_col='.$col_id.' WHERE ro.ro_ex IN ('.get_exes($rows_ids,$ex1).') AND ro.owner_id IN ('.$rows_ids.') AND ro.ro_enable=1 ORDER BY rw.value_value'.$type.', ro.ro_pos'.$type.$limit;
				$rs=getall($db,$query,2);
				$res=rows_to_rlink2($rs);
			} else $res=Array();
		} else $res=Array();
		return $res;
};
$cmd['stasc'][CMD_ARRAY]->result=CMD_ARRAY;
$cmd['stasc'][CMD_ARRAY]->result_long=CMD_ROW;
$cmd['stdesc'][CMD_ARRAY]->alias=&$cmd['stasc'][CMD_ARRAY];
$cmd['stasc'][CMD_ARRAY]->visual=Array('title'=>'выдать сортированные объекты подтаблицы','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>1),/* разбирается не из PUT, а из next_op, согласно правилу SP_POINT */
		1=>Array('title'=>'поле сортировки (по возрастанию)','type'=>STRING_COLNAME_ANY,'in_quotes'=>1,'req'=>1),
		2=>Array('title'=>'начальный элемент','type'=>CMD_STRING,'req'=>0),
		3=>Array('title'=>'количество','type'=>CMD_STRING,'req'=>0)
),'special'=>SP_POINT,'group'=>GROUP_ARR_SUB);


$cmd['dump'][CMD_ARRAY]->process=function(&$data,&$op,&$ops){
		$t=start_buffer();
		var_dump($data);
		return return_buffer($t);
};
$cmd['dump'][CMD_ARRAY]->result=CMD_STRING;
$cmd['dump'][CMD_ARRAY]->visual=Array('title'=>'Вывести дамп переменной','hidden'=>1);


?>