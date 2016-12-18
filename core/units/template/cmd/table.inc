<?php

define('GROUP_TBL_SPEC',5001);
$cmd_group[GROUP_TBL_SPEC]=Array('title'=>'Специфические','pos'=>20);
define('GROUP_TBL_OBJ',5002);
$cmd_group[GROUP_TBL_OBJ]=Array('title'=>'Объекты','pos'=>3);
define('GROUP_TBL_FILTER',5003);
$cmd_group[GROUP_TBL_FILTER]=Array('title'=>'Фильтры','pos'=>4);
define('GROUP_TBL_SUB',5004);
$cmd_group[GROUP_TBL_SUB]=Array('title'=>'Подтаблицы','pos'=>5);
define('GROUP_TBL_COL',5005);
$cmd_group[GROUP_TBL_COL]=Array('title'=>'Поля','pos'=>6);
define('GROUP_TBL_PARAM',5006);
$cmd_group[GROUP_TBL_PARAM]=Array('title'=>'Параметры','pos'=>7);
define('GROUP_TBL_ARR',5007);
$cmd_group[GROUP_TBL_ARR]=Array('title'=>'Массивы','pos'=>8);
define('GROUP_TBL_OPS',5008);
$cmd_group[GROUP_TBL_OPS]=Array('title'=>'Операции','pos'=>9);


function seek_table($table){
	global $stable,$db,$rlink;
	$ot=$table;
	$ex=0;
	$tex=0;
	$own=0;
	$row=0;
	if(!is_object($table) && !is_numeric($table) && is_string($table)){
		if(strstr5($table,':')){
			$table=explode(':',$table);
			$ex=$table[1];
			$own=$table[2];
			$tex=$table[4];
			$table=$table[0];
		}
	}
	if(is_object($table) && isset($table->id)){
		if(strstr($table->id,':')){
			$tmp=explode(':',$table->id);
			$table->id=$tmp[0];
			$ex=$tmp[1];
			if(isset($tmp[2])) $own=$tmp[2];
			if(isset($tmp[4])) $tex=$tmp[4];
		}
		if(isset($table->ex)) $ex=$table->ex;
		if(isset($table->tex)) $tex=$table->tex;
		if(isset($table->own)) $own=$table->own;
		if(isset($table->row)) /*$own*/$row=$table->row;
		$table=$table->id;
	}
	if(empty($ex) && !empty($own)){
		seek_rlink($own); global $rlink;
		$ex=get_ex2($rlink[$own]->tex);
	}
	if(!empty($ex) && empty($tex)){
		$tex=get_tex($own,$ex,$table);
	}
	if(!isset($stable[$table])){
		if(!empty($table)){
			getrow($db,"SELECT * FROM main_table WHERE table_id=$table",1,'main_table',__LINE__,__FILE__);
			$stable[$table]=$db->Record;
		} else $stable[$table]=0;
	}
	return Array(	'table'=>$stable[$table],
				'ex'=>$ex,
				'tex'=>$tex,
				'own'=>$own,
				'row'=>$row,
				'id'=>$table);
}


$cmd['rows'][CMD_TABLE]->process=function(&$data,&$op,&$ops,$type='rows'){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		global $utpl,$gsres;
		if(isset($ops[key($ops)]) && $ops[key($ops)]->type==LEXER_CMD && !empty($ops[key($ops)]->data) && $ops[key($ops)]->data=='id'){
			return $table.':'.$ex.':'.$owner.':'.$stable;
		}
		$GLOBALS["ex_ex2"]=$ex;
		$gs->rows=Array();
		$gs->ex=$ex;
		$ex1=get_tex($owner,$ex,$table);
		$utpl["rows"][$table.'.'.$ex1]=1;
		if($ex==0){
			$ex=get_ex2($ex1);
		}
		if($ex==0) $seek_ex=0; else $seek_ex=1;
		$seek_auth=0;
		if($type=='rows') $seek_ena=1; //только активные
		if($type=='rows2' || $type=='rows_all') $seek_ena=0; //все
		if($type=='rows3' || $type=='rows_na') $seek_ena=2;	//неактивные
		if($type=='arows' || $type=='rows_auth'){//проверка аутентификации
			$seek_ena=1;
			$seek_auth=1;
		}
		$ignore_table=0;
		
		if(empty($dval)){
			if(!empty($owner) && isset($GLOBALS["spec_step2"]) && !empty($GLOBALS["current"]) && $GLOBALS["current"]==$owner){
				global $tree_vars2;
				$tree_vars2[$GLOBALS["spec_step2"]][$table]=$table.'.'.$seek_ena.'.'.$seek_ex.'.'.$ex.'.'.$ex1.'.'.$ignore_table.'.'.$seek_auth;
			}
			if(!isset($gsres[$owner][$table][$seek_ena][$seek_ex][$ex][$ex1][$ignore_table][$seek_auth])){
				$gs->rows=get_sub($owner,$table,$seek_ena,$seek_ex,$ignore_table,0,/*1*/0,$ex1,$ex,$table,0,$table,$seek_auth);
				$gsres[$owner][$table][$seek_ena][$seek_ex][$ex][$ex1][$ignore_table][$seek_auth]=$gs->rows;
			} else $gs->rows=$gsres[$owner][$table][$seek_ena][$seek_ex][$ex][$ex1][$ignore_table][$seek_auth];
		} else $gs->rows=$dval;
		$gs->table=$table;
		$tex=get_tex($owner,$ex,$table);
		$gs->tex=$tex;
		$gs->owner=$owner;
		if(empty($stable)) $stable=getrowval("SELECT table_id, table_sname FROM main_table WHERE table_id=$table","table_sname");
		$gs->stable=$stable;
		return $gs;
};
$cmd['rows'][CMD_TABLE]->result=CMD_ARRAY;
$cmd['rows'][CMD_TABLE]->result_long=CMD_ROW;
$cmd['row'][CMD_TABLE]->alias=&$cmd['rows'][CMD_TABLE];
$cmd['rows2'][CMD_TABLE]->alias=&$cmd['rows'][CMD_TABLE];
$cmd['rows_all'][CMD_TABLE]->alias=&$cmd['rows'][CMD_TABLE];
$cmd['rows3'][CMD_TABLE]->alias=&$cmd['rows'][CMD_TABLE];
$cmd['rows_na'][CMD_TABLE]->alias=&$cmd['rows'][CMD_TABLE];
$cmd['arows'][CMD_TABLE]->alias=&$cmd['rows'][CMD_TABLE];
$cmd['rows_auth'][CMD_TABLE]->alias=&$cmd['rows'][CMD_TABLE];
$cmd['rows'][CMD_TABLE]->visual=Array('title'=>'активные объекты','pos'=>2);
$cmd['rows_all'][CMD_TABLE]->visual=Array('title'=>'все объекты','group'=>GROUP_TBL_OBJ);
$cmd['rows_na'][CMD_TABLE]->visual=Array('title'=>'деактивированные объекты','group'=>GROUP_TBL_OBJ);
$cmd['rows_auth'][CMD_TABLE]->visual=Array('title'=>'доступные пользователю объекты','group'=>GROUP_TBL_OBJ);


$cmd['irows'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		global $db;
		$texs=implode(',',getall3($db,"SELECT * FROM ex_group WHERE ex_ex2=$ex","ex_ex1"));
		$rows=getall($db,"SELECT * FROM row_owner WHERE ro_ex IN ($texs) AND row_table=$table");
		return db_to_rows($rows);
};
$cmd['irows'][CMD_TABLE]->result=CMD_ARRAY;
$cmd['irows'][CMD_TABLE]->result_long=CMD_ROW;
$cmd['rows4'][CMD_TABLE]->alias=&$cmd['irows'][CMD_TABLE];
$cmd['irows'][CMD_TABLE]->visual=Array('title'=>'все объекты данной таблицы в рамках текущего экземпляра модуля','group'=>GROUP_TBL_OBJ);


$cmd['st'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		//array_unshift2($ops,create_op('st',LEXER_CMD,'.'));
		prev($ops);
		global $cmd;
		//$x=&$cmd['rows'][CMD_TABLE]->process;
		$x=get_cmd_link('rows',CMD_TABLE);
		return $x($data,$op,$ops);
};
$cmd['st'][CMD_TABLE]->result=CMD_ARRAY;
$cmd['st'][CMD_TABLE]->result_long=CMD_ROW;
$cmd['st2'][CMD_TABLE]->alias=&$cmd['st'][CMD_TABLE];
$cmd['st_all'][CMD_TABLE]->alias=&$cmd['st'][CMD_TABLE];
$cmd['st3'][CMD_TABLE]->alias=&$cmd['st'][CMD_TABLE];
$cmd['st_na'][CMD_TABLE]->alias=&$cmd['st'][CMD_TABLE];
$cmd['ast'][CMD_TABLE]->alias=&$cmd['st'][CMD_TABLE];
$cmd['st_auth'][CMD_TABLE]->alias=&$cmd['st'][CMD_TABLE];
$cmd['st'][CMD_TABLE]->visual=Array('title'=>'активные элементы подтаблицы','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_TBL_SUB);
$cmd['st_all'][CMD_TABLE]->visual=Array('title'=>'все элементы подтаблицы','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_TBL_SUB);
$cmd['st_na'][CMD_TABLE]->visual=Array('title'=>'не активные элементы подтаблицы','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_TBL_SUB);
$cmd['st_auth'][CMD_TABLE]->visual=Array('title'=>'элементы подтаблицы, доступные для пользователя','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_TBL_SUB);


$cmd['owner'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return prepend_type($owner,'row',Array('table'=>$table));
};
$cmd['owner'][CMD_TABLE]->result=CMD_ROW;
$cmd['owner'][CMD_TABLE]->visual=Array('title'=>'Объект-родитель','group'=>GROUP_TBL_PARAM);


$cmd['fastget'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		if(empty($tex)) $tex=get_tex($owner,$ex,$table);
		return fastget($op,$ops,Array(),$table,$tex,$owner);
};
$cmd['fastget'][CMD_TABLE]->result=CMD_ARRAY;
$cmd['fastget'][CMD_TABLE]->result_long=CMD_ROW;
$cmd['fastget2'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['fastget3'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['ifastget'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['ifastget2'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['ifastget3'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];

$cmd['efastget'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['efastget2'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['efastget3'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['eifastget'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['eifastget2'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['eifastget3'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];

$cmd['afastget'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['afastget2'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['afastget3'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['aifastget'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['aifastget2'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['aifastget3'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];


$cmd['fastgetc'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		if(empty($tex)) $tex=get_tex($owner,$ex,$table);
		return fastget($op,$ops,Array(),$table,$tex,$owner);
};
$cmd['fastgetc'][CMD_TABLE]->result=CMD_STRING;
$cmd['fastget2c'][CMD_TABLE]->alias=&$cmd['fastgetc'][CMD_TABLE];
$cmd['fastget3c'][CMD_TABLE]->alias=&$cmd['fastgetc'][CMD_TABLE];
$cmd['ifastgetc'][CMD_TABLE]->alias=&$cmd['fastgetc'][CMD_TABLE];
$cmd['ifastget2c'][CMD_TABLE]->alias=&$cmd['fastgetc'][CMD_TABLE];
$cmd['ifastget3c'][CMD_TABLE]->alias=&$cmd['fastgetc'][CMD_TABLE];

$cmd['efastgetc'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['efastget2c'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['efastget3c'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['eifastgetc'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['eifastget2c'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['eifastget3c'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];

$cmd['afastgetc'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['afastget2c'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['afastget3c'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['aifastgetc'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['aifastget2c'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];
$cmd['aifastget3c'][CMD_TABLE]->alias=&$cmd['fastget'][CMD_TABLE];

$cmd['fastget'][CMD_TABLE]->visual=Array('title'=>'быстрая выборка','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>0),
		1=>Array('title'=>'условие','type'=>CMD_FASTGET_IF,'req'=>1),
		2=>Array('title'=>'сортировка','type'=>CMD_FASTGET_ORDER,'req'=>0),
		3=>Array('title'=>'начальный элемент','type'=>CMD_STRING,'req'=>0),
		4=>Array('title'=>'количество','type'=>CMD_STRING,'req'=>0)
),'special'=>SP_FASTGET,'group'=>GROUP_TBL_FILTER,'hidden'=>1);
$cmd['fastgetc'][CMD_TABLE]->visual=Array('title'=>'быстрая выборка (количество)','put'=>Array(
		0=>Array('title'=>'подтаблица','type'=>STRING_SUBTABLE_NAMES,'req'=>0),
		1=>Array('title'=>'условие','type'=>CMD_FASTGET_IF,'req'=>1),
		2=>Array('title'=>'сортировка','type'=>CMD_FASTGET_ORDER,'req'=>0),
		3=>Array('title'=>'начальный элемент','type'=>CMD_STRING,'req'=>0),
		4=>Array('title'=>'количество','type'=>CMD_STRING,'req'=>0)
),'special'=>SP_FASTGET,'group'=>GROUP_TBL_FILTER,'result'=>STRING_NUM,'hidden'=>1);


$cmd['foreign_tables'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		//$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		$dst_module_id=ex2m($ex);
		if($op->data=='foreign_tables') $ignore_module=1;
		if($op->data=='local_tables') $ignore_module=2;
		if($op->data=='foreign_tables'){
			$op=array_shiftA($ops);
			$module_sname=$op->data;
			$src_module_id=getrowval("SELECT module_id, module_sname FROM main_module WHERE module_sname='$module_sname'",'module_id');
		} else {
			$src_module_id=$dst_module_id;
		}
		$op=array_shiftA($ops);
		$table_sname=$op->data;
		if(empty($tex)) $tex=get_tex($owner,$ex,$table);		
		$tables=get_crossmodule_subtable($dst_module_id,$ignore_module,1);
		if(!empty($tables)) foreach($tables AS $tbl) if($tbl["table_module"]==$src_module_id && $tbl["table_sname"]==$table_sname){
			//echo 'res_table = '.$tbl["table_id"].'<br>ex='.$ex.'<br>tex='.$tex.'<br>table_sname='.$table_sname.'<br>module_sname='.$module_sname.'<br>src_module='.$src_module_id.'<br>dst_module='.$dst_module_id.'<br><br>';
			return prepend_type($tbl['table_id'],'tbl',Array('ex'=>$ex,'tex'=>$tex,'sname'=>$table_sname,'own'=>$owner));
		}
};
$cmd['foreign_tables'][CMD_TABLE]->result=CMD_TABLE;
$cmd['local_tables'][CMD_TABLE]->alias=&$cmd['foreign_tables'][CMD_TABLE];
$cmd['foreign_tables'][CMD_TABLE]->visual=Array('title'=>'Доступ к подтаблицам из других модулей','put'=>Array(
		0=>Array('title'=>'Модуль','type'=>STRING_MODULE_SNAME,'set_attr'=>1,'req'=>1),
		1=>Array('title'=>'Таблица','type'=>STRING_TABLE_NAME,'set_attr'=>1,'req'=>1)
),'special'=>SP_DOUBLE_POINT,'group'=>GROUP_TBL_SUB);
$cmd['local_tables'][CMD_TABLE]->visual=Array('title'=>'Доступ к подтаблицам из текущего модуля','put'=>Array(
		0=>Array('title'=>'Таблица','type'=>STRING_TABLE_NAME,'set_attr'=>1,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_TBL_SUB);


$cmd['allcol'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		$rows=allcol($op,$table,$ex,$owner);
		return $rows;
};
$cmd['allcol'][CMD_TABLE]->result=CMD_ARRAY;
$cmd['allcol'][CMD_TABLE]->result_long=CMD_COL;
$cmd['allcol2'][CMD_TABLE]->alias=&$cmd['allcol'][CMD_TABLE];
$cmd['allcol_uniq'][CMD_TABLE]->alias=&$cmd['allcol'][CMD_TABLE];
$cmd['allcol'][CMD_TABLE]->visual=Array('title'=>'Массив данных из определённого поля','put'=>Array(
		0=>Array('title'=>'Поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>1)
),'group'=>GROUP_TBL_ARR);
$cmd['allcol_uniq'][CMD_TABLE]->visual=Array('title'=>'Массив данных из определённого поля (без дубликатов)','put'=>Array(
		0=>Array('title'=>'Поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>1)
),'group'=>GROUP_TBL_ARR);


$cmd['rasc'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		global $db;
		if($op->data=='rasc') $type='';
		if($op->data=='rdesc') $type=' DESC';
		
		$ex1=get_tex($owner,$ex,$table);
		$utpl["rows"][$table.'.'.$ex1]=1;
		
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$col_sname=parse_var($op->put[0]);
		$col_id=getrowval("SELECT col_sname, col_id, col_table FROM main_col WHERE col_table=$table AND col_sname='$col_sname'","col_id");
		if(count($op->put)==2){
			$start=0;
			$end=parse_var($op->put[1]);
			$end=round($end);
			if($start<0) $start=0;
			if($end<0) $end=0;
			$limit=' LIMIT '.$start.','.$end;
		} else if(count($op->put)==3){
			$start=parse_var($op->put[1]);
			$end=parse_var($op->put[2]);
			$start=round($start);
			$end=round($end);	
			if($start<0) $start=0;
			if($end<0) $end=0;
			$limit=' LIMIT '.$start.','.$end;
		} else {
			$start=0;
			$end=0;
			$limit='';
		}
		$orde='';
		if($type==''){
			$orde='rw.value_value=0, -rw.value_value DESC, rw.value_value';
		} else {
			$orde='rw.value_value!=0, -rw.value_value, rw.value_value DESC';
		}
		$query='SELECT ro.*, rw.value_value, rw.value_row, rw.value_col FROM row_owner AS ro LEFT JOIN row_value AS rw ON ro.row_id=rw.value_row AND rw.value_col='.$col_id.' WHERE ro.ro_ex IN ('.get_exes($owner,$ex1).') AND ro.owner_id='.$owner.' AND ro.ro_enable=1 ORDER BY '.$orde.', ro.ro_pos'.$type.$limit;
		$rs=getall($db,$query,2);
		etf('parse_table');
		if(!empty($rs)) foreach($rs AS $row)if(!empty($row["value_row"])) $utpl["row"][$row["value_row"]]=1;
		$rows=rows_to_rlink2($rs);
		return $rows;
};
$cmd['rasc'][CMD_TABLE]->result=CMD_ARRAY;
$cmd['rasc'][CMD_TABLE]->result_long=CMD_ROW;
$cmd['rdesc'][CMD_TABLE]->alias=&$cmd['rasc'][CMD_TABLE];
$cmd['rasc'][CMD_TABLE]->visual=Array('title'=>'Массив объектов, сортированных по полю','put'=>Array(
		0=>Array('title'=>'Поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>1),
		1=>Array('title'=>'Пропустить элементов','type'=>CMD_STRING,'req'=>0),
		2=>Array('title'=>'Количество','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_TBL_ARR);
$cmd['rdesc'][CMD_TABLE]->visual=Array('title'=>'Массив объектов, сортированных по полю (обратная сортировка)','put'=>Array(
		0=>Array('title'=>'Поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>1),
		1=>Array('title'=>'Пропустить элементов','type'=>CMD_STRING,'req'=>0),
		2=>Array('title'=>'Количество','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_TBL_ARR);


$cmd['count'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		global $db,$rlink;
		if(empty($tex)) $ex1=get_tex($owner,$ex,$table); else $ex1=$tex;
		$utpl["rows"][$table.'.'.$ex1]=1;
		if(empty($ex1) && !empty($owner)){
			seek_rlink($owner); global $rlink;
			$ex1=$rlink[$owner]->tex;
		}
		if(!empty($owner)){
			$query='SELECT count(ro_id) AS cnt FROM row_owner WHERE owner_id='.$owner.' AND row_table='.$table.' AND ro_enable=1';
		} else {
			$query='SELECT count(ro_id) AS cnt FROM row_owner WHERE ro_ex IN ('.get_exes($owner,$ex1).') AND owner_id='.$owner.' AND row_table='.$table.' AND ro_enable=1';
		}
		getrow($db,$query,1,"row_owner");
		if(isset($db->Record["cnt"])) $x=$db->Record["cnt"]; else $x=0;
		return $x;
};
$cmd['count'][CMD_TABLE]->result=CMD_STRING;
$cmd['count'][CMD_TABLE]->visual=Array('title'=>'количество объектов','group'=>GROUP_TBL_OBJ,'result'=>STRING_NUM);


$cmd['ex'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return prepend_type($ex,'ex',Array('owner'=>(!empty($row)?$row:$owner)));
};
$cmd['ex'][CMD_TABLE]->result=CMD_EX;
$cmd['ex'][CMD_TABLE]->visual=Array('title'=>'Экземпляр модуля','group'=>GROUP_TBL_PARAM);


$cmd['add'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		
		global $prepost;
		if(!empty($op->put)){
			check_single($op->put,$GLOBALS["lex_func_sep"]);
			//testfile(serialize($op->put));
			foreach($op->put AS $o){
				if(!is_array($o)) $o=$o->sub;
				if($o[0]->next_operation=='='){
					$var=parse_var($o[0]);//$o[0]->data
					if($var=='major'){
						$m=seek_major($table);
						$var=$m['col_sname'];
					}
					if(!empty($o[1]->sub) && $o[1]->sub[0]->next_operation==','){
						$value=Array();
						foreach($o[1]->sub AS $os){
							$t=parse_var($os);
							if(is_object($t) && isset($t->id)) $t=$t->id;
							$value[$t]=1;
						}
					} else {
						$value=parse_var($o[1]);
						if(is_object($value) && isset($value->rows)) $value=$value->rows;
						if(is_array($value)){
							$arr=$value;
							$value=Array();
							foreach($arr AS $a) if(is_object($a) && isset($a->id)) $arr[$a->id]=1; else $arr[$a]=1;
							$value=$arr;
						} else {
							if(is_object($value) && isset($value->id)) $value=$value->id;
						}
					}
					$prepost[$var]->val=$value;
					$prepost[$var]->autoload=false;
				}
			}
		}
		
		$table_sname='';
		$tbl=$table.':'.$ex.':'.$owner.':'.$table_sname.':'.$tex;
		$c=find_cmd('addrow',CMD_BASE,$tbl);
		$tmp=do_cmd($op,$ops,$c,$tbl);
		
		return false;
};
$cmd['add'][CMD_TABLE]->result=CMD_NONE;
$cmd['add'][CMD_TABLE]->visual=Array('title'=>'Добавить объект','group'=>GROUP_TBL_OPS,'special'=>SP_ADD_COLS);
$cmd['addrow'][CMD_TABLE]->alias=&$cmd['addrow'][CMD_TABLE];


$cmd['self'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		$res->name=$t["table_name"];
		$res->sname=$t["table_sname"];
		$res->id=$table.':'.$ex;
		$res->owner=$owner;
		if(!empty($row)) $res->row=$row;
		$res->tex=get_tex($owner,$ex,$table);
		$res->type='tbl';
		return $res;
};
$cmd['self'][CMD_TABLE]->result=CMD_TABLE;


$cmd['name'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return $t['table_name'];
};
$cmd['name'][CMD_TABLE]->result=CMD_STRING;
$cmd['name'][CMD_TABLE]->visual=Array('title'=>'Имя таблицы','group'=>GROUP_TBL_PARAM);


$cmd['sname'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return $stable;
};
$cmd['sname'][CMD_TABLE]->result=CMD_STRING;
$cmd['sname'][CMD_TABLE]->visual=Array('title'=>'Спец.имя таблицы','group'=>GROUP_TBL_PARAM);


$cmd['on_edit'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return $t['table_onedit'];
};
$cmd['on_edit'][CMD_TABLE]->result=CMD_STRING;
$cmd['on_edit'][CMD_TABLE]->visual=Array('title'=>'Шаблон действий при изменении объекта','result'=>CMD_TPL,'group'=>GROUP_TBL_SPEC);


$cmd['on_top'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return $t['table_top'];
};
$cmd['on_top'][CMD_TABLE]->result=CMD_STRING;
$cmd['on_top'][CMD_TABLE]->visual=Array('title'=>'Шаблон шапки таблицы','result'=>CMD_TPL,'group'=>GROUP_TBL_SPEC);


$cmd['module'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return prepend_type($t['table_module'],'md',Array('owner'=>(!empty($row)?$row:$owner)));
};
$cmd['module'][CMD_TABLE]->result=CMD_MODULE;
$cmd['module'][CMD_TABLE]->visual=Array('title'=>'Модуль таблицы','group'=>GROUP_TBL_PARAM);


$cmd['on_bottom'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return $t['table_bottom'];
};
$cmd['on_bottom'][CMD_TABLE]->result=CMD_STRING;
$cmd['on_bottom'][CMD_TABLE]->visual=Array('title'=>'Шаблон подвала таблицы','result'=>CMD_TPL,'group'=>GROUP_TBL_SPEC);


$cmd['md'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		global $rlink;
		$tmodule=$t["table_module"];
		if(!empty($owner)){
			global $rlink;
			$ownerx=seek_top_owner($owner);
			seek_rlink_ex($ownerx);
			$x=$rlink[$ownerx]->tex;
			$ex=get_ex2($x);
		}
		if(empty($ex) && !empty($owner)) $ex=$owner;
		
		//может замедлить работу
		global $lu_back, $left_url, $right_url;
		$tmp=shell_module($tmodule.':'.$ex.'?'.$owner,'',1); // см. в ЕЕвв по вопросу md //вроде проблема решена skip_shell
		if(!empty($lu_back)){$left_url=$lu_back;$right_url=$ru_back;}
		return prepend_type($tmodule,'md',Array('ex'=>$ex,'owner'=>(!empty($row)?$row:$owner)));
};
$cmd['md'][CMD_TABLE]->result=CMD_MODULE;
$cmd['md'][CMD_TABLE]->visual=Array('title'=>'Доступ к модулю, связанного с таблицей (включает запуск этого модуля)','group'=>GROUP_TBL_SPEC);


$cmd['add?'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		$ex2=$ex;//get_ex2($tex);
		if(empty($owner)){
			$chk=check_row(0,$table,$ex2,'add');
		} else {
			$chk=check_operation('add',0,$owner,$ex2,$table);
		}
		return $chk;
		//return check_row(0,$table,$ex,'add',0);
};
$cmd['add?'][CMD_TABLE]->result=CMD_STRING;
$cmd['add?'][CMD_TABLE]->visual=Array('title'=>'Можно добавлять объекты?','group'=>GROUP_TBL_PARAM);


$cmd['sel'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		global $url_row;
		if(!empty($url_row[$table])){
			return prepend_type($url_row[$table],'row',Array('table'=>$table));
		}
};
$cmd['sel'][CMD_TABLE]->result=CMD_ROW;
$cmd['sel_row'][CMD_TABLE]->alias=&$cmd['sel'][CMD_TABLE];
$cmd['sel'][CMD_TABLE]->visual=Array('title'=>'Текущий объект (определённый через URL разбор)','group'=>GROUP_TBL_OBJ);


$cmd['id'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return $table.':'.$ex.':'.$owner.':'.$t['table_sname'].':'.get_tex($owner,$ex,$table);
};
$cmd['id'][CMD_TABLE]->result=CMD_STRING;
$cmd['id'][CMD_TABLE]->visual=Array('title'=>'Полный идентификатор','group'=>GROUP_TBL_PARAM);


$cmd['eid'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return get_tex($owner,$ex,$table);
};
$cmd['eid'][CMD_TABLE]->result=CMD_STRING;
$cmd['eid'][CMD_TABLE]->visual=Array('title'=>'Идентификатор экземпляра таблицы','hidden'=>1);


$cmd['rid'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return $table;
};
$cmd['rid'][CMD_TABLE]->result=CMD_STRING;
$cmd['rid'][CMD_TABLE]->visual=Array('title'=>'Идентификатор','hidden'=>1);


$cmd['url'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		$c=url_col($table);
		return prepend_type($c['col_id'],'cl',Array('sname'=>$c['col_sname'],'row'=>(!empty($row)?$row:$owner)));
};
$cmd['url'][CMD_TABLE]->result=CMD_COL;
$cmd['geturl'][CMD_TABLE]->alias=&$cmd['url'][CMD_TABLE];
$cmd['url'][CMD_TABLE]->visual=Array('title'=>'Поле URL','group'=>GROUP_TBL_COL);


$cmd['major'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		$c=seek_major($table);
		return prepend_type($c['col_id'],'cl',Array('sname'=>$c['col_sname'],'row'=>(!empty($row)?$row:$owner)));
};
$cmd['major'][CMD_TABLE]->result=CMD_COL;
$cmd['major'][CMD_TABLE]->visual=Array('title'=>'Основное поле','group'=>GROUP_TBL_COL);


$cmd['owner'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return prepend_type((empty($row)?$owner:$row),'row');
};
$cmd['owner'][CMD_TABLE]->result=CMD_ROW;
$cmd['owner'][CMD_TABLE]->visual=Array('title'=>'Объект-владелец (для подтаблиц)','group'=>GROUP_TBL_PARAM);


$cmd['cols'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		global $cl_cache,$pccol,$db;
		if(!isset($cl_cache[$table])){
			$cls=getall($db,"SELECT * FROM main_col WHERE col_table=$table",1,"main_col",__LINE__,__FILE__);
			foreach($cls AS $cl) $pccol[$cl["col_id"]]=$cl;
			$cl_cache[$table]=$cls;
		} else $cls=$cl_cache[$table];
		$res=Array();$cr=0;
		foreach($cls AS $cl){
			$res[$cr]->id=$cl["col_id"];
			$res[$cr]->name=$cl["col_name"];
			$res[$cr]->sname=$cl["col_sname"];
			$res[$cr]->type='cl';
			$res[$cr]->row=(!empty($row)?$row:$owner);
			$cr++;
		}
		return $res;
};
$cmd['cols'][CMD_TABLE]->result=CMD_ARRAY;
$cmd['cols'][CMD_TABLE]->result_long=CMD_COL;
$cmd['cols'][CMD_TABLE]->visual=Array('title'=>'Массив полей','group'=>GROUP_TBL_COL);


$cmd['col'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		if(!empty($op->put)){
			$col_sname=parse_var($op->put);
		} else {
			$bop=array_shiftA($ops);
			$col_sname=$bop->data;
		}
		$col_id=getrowval("SELECT col_sname, col_id, col_table FROM main_col WHERE col_table=$table AND col_sname='$col_sname'",'col_id');
		if(empty($col_id)) return false;
		return prepend_type($col_id,'cl',Array('sname'=>$col_sname,'row'=>(!empty($row)?$row:$owner)));
};
$cmd['col'][CMD_TABLE]->result=CMD_COL;
$cmd['col'][CMD_TABLE]->visual=Array('title'=>'Доступ к полю','put'=>Array(
		0=>Array('title'=>'Поле','type'=>/*CMD_COLNAME*/STRING_COLNAME,'req'=>1)
),'special'=>SP_POINT,'group'=>GROUP_TBL_COL);


$cmd['admin_url'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		return get_admin_url(0,$table,$tex,$owner);
};
$cmd['admin_url'][CMD_TABLE]->result=CMD_STRING;
$cmd['aurl'][CMD_TABLE]->alias=&$cmd['admin_url'][CMD_TABLE];
$cmd['admin_url'][CMD_TABLE]->visual=Array('title'=>'Ссылка на таблицу данных в кабинете администратора','group'=>GROUP_TBL_SPEC);


$cmd['gets'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		$x=get_cmd_link('get',CMD_ARRAY);
		if(empty($tex)) $tex=get_tex($owner,$ex,$table);
		return $x($data,$op,$ops,1,$table,$tex,$owner);
};
$cmd['get'][CMD_TABLE]->alias=&$cmd['gets'][CMD_TABLE];
$cmd['get'][CMD_TABLE]->result=CMD_ROW;
$cmd['gets'][CMD_TABLE]->result=CMD_ARRAY;
$cmd['gets'][CMD_TABLE]->result_long=CMD_ROW;
$cmd['gets'][CMD_TABLE]->visual=Array('title'=>'выдать элементы по значению поля','pos'=>1000,'put'=>Array(
		0=>Array('title'=>'поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>0),
		1=>Array('title'=>'значение','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_TBL_FILTER);
$cmd['get'][CMD_TABLE]->visual=Array('title'=>'выдать элемент по значению поля','pos'=>1001,'put'=>Array(
		0=>Array('title'=>'поле','type'=>STRING_COLNAME,'in_quotes'=>1,'req'=>0),
		1=>Array('title'=>'значение','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_TBL_FILTER);


$cmd['find'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		if(empty($tex)) $tex=get_tex($owner,$ex,$table);
		
		return fastget($op,$ops,Array(),$table,$tex,$owner);
};
$cmd['find'][CMD_TABLE]->result=CMD_ARRAY;
$cmd['find'][CMD_TABLE]->visual=Array('title'=>'Выбрать объекты','put'=>Array(
		0=>Array('title'=>'Игнорировать родителя','type'=>CMD_LOGICAL,'static'=>'ignore_owner','req'=>0),
		1=>Array('title'=>'Проверять доступы','type'=>CMD_LOGICAL,'static'=>'check_auth','req'=>0),
		2=>Array('title'=>'Подтаблица','type'=>STRING_SUBTABLE_NAMES,'sname'=>'subtable','req'=>0),
		3=>Array('title'=>'Условия поиска','type'=>CMD_STRING,'list'=>Array(''=>'Нет','ignore_ex'=>'Игнорировать экземпляр таблицы','ignore_module'=>'Игнорировать модуль'),'req'=>0),
		4=>Array('title'=>'Какие объекты возвращать','type'=>CMD_STRING,'list'=>Array(''=>'Активные','disabled'=>'Не активные','all'=>'Все'),'req'=>0),
		5=>Array('title'=>'Дополнительные условия','type'=>CMD_STRING,'list'=>Array(''=>'Нет','return_owners'=>'Вернуть родителей найденных объектов'),'req'=>0)
),'group'=>GROUP_TBL_FILTER,'pos'=>0,'result'=>CMD_FIND);


$cmd['where'][CMD_FIND]->visual=Array('title'=>'Условие','put'=>Array(
		0=>Array('title'=>'Поле','type'=>STRING_COLNAME_FIND,'req'=>1),
		1=>Array('title'=>'Операция','type'=>CMD_STRING,'list'=>Array(
			"'=='"=>'==',
			"'!='"=>'!=',
			"'<'"=>'<',
			"'<='"=>'<=',
			"'>'"=>'>',
			"'>='"=>'>=',
			'contain'=>'поле содержит значение',
			'not_contain'=>'поле не содержит значения',
			'contained'=>'значение содержит поле',
			'not_contained'=>'значение не содержит поля',
			'in_array'=>'поле содержится в массиве',
			'not_in_array'=>'поле не содержится в массиве',
			'empty'=>'пустое',
			'not_empty'=>'не пустое'
		),'req'=>1),
		2=>Array('title'=>'Значение','type'=>CMD_STRING,'req'=>0),
		3=>Array('title'=>'Мутатор поля','type'=>CMD_STRING,'list'=>Array(''=>'Нет','mutate_lower'=>'В нижнем регистре','mutate_length'=>'Работать с длинной поля'),'req'=>0),
),'pos'=>2,'result'=>CMD_FIND);


$cmd['where_set'][CMD_FIND]->visual=Array('title'=>'Набор условий','put'=>Array(
		0=>Array('title'=>'Тип проверки условий','type'=>CMD_STRING,'list'=>Array('and'=>'и','or'=>'или'),'req'=>1),
		1=>Array('title'=>'Условия','type'=>-CMD_FIND_WHERE,'is_array'=>1,'req'=>1)
),'pos'=>3,'result'=>CMD_FIND);


$cmd['where_set'][CMD_FIND_WHERE]=&$cmd['where_set'][CMD_FIND];
$cmd['where'][CMD_FIND_WHERE]=&$cmd['where'][CMD_FIND];


$cmd['sort'][CMD_FIND]->visual=Array('title'=>'Сортировать','put'=>Array(
		0=>Array('title'=>'Поле для сортировки','type'=>STRING_COLNAME_FIND,'req'=>1),
		1=>Array('title'=>'Тип сортировки','type'=>CMD_STRING,'list'=>Array(''=>'По возрастанию','desc'=>'По убыванию','rand'=>'Случайным образом'),'req'=>0),
),'pos'=>4,'result'=>CMD_FIND);


$cmd['start'][CMD_FIND]->visual=Array('title'=>'Смещение','put'=>Array(
		0=>Array('title'=>'Пропустить объектов','type'=>CMD_STRING,'req'=>1),
),'pos'=>5,'result'=>CMD_FIND);


$cmd['limit'][CMD_FIND]->visual=Array('title'=>'Лимит','put'=>Array(
		0=>Array('title'=>'Лимит объектов','type'=>CMD_STRING,'req'=>1),
),'pos'=>6,'result'=>CMD_FIND);


$cmd['do'][CMD_FIND]->visual=Array('title'=>'Вернуть результат','pos'=>1000,'special'=>SP_IGNORE,'result'=>CMD_ARRAY);


$cmd['find_count'][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		if(empty($tex)) $tex=get_tex($owner,$ex,$table);
		
		return fastget($op,$ops,Array(),$table,$tex,$owner);
};
$cmd['find_count'][CMD_TABLE]->result=CMD_STRING;//STRING_NUM
$cmd['find_count'][CMD_TABLE]->visual=Array('title'=>'Выдать количество объектов','put'=>Array(
		0=>Array('title'=>'Игнорировать родителя','type'=>CMD_LOGICAL,'static'=>'ignore_owner','req'=>0),
		1=>Array('title'=>'Подтаблица','type'=>STRING_SUBTABLE_NAMES,'sname'=>'subtable','req'=>0),
		2=>Array('title'=>'Условия поиска','type'=>CMD_STRING,'list'=>Array(''=>'Нет','ignore_ex'=>'Игнорировать экземпляр таблицы','ignore_module'=>'Игнорировать модуль'),'req'=>0),
		3=>Array('title'=>'Какие объекты возвращать','type'=>CMD_STRING,'list'=>Array(''=>'Активные','disabled'=>'Не активные','all'=>'Все'),'req'=>0),
		4=>Array('title'=>'Дополнительные условия','type'=>CMD_STRING,'list'=>Array(''=>'Нет','return_owners'=>'Вернуть родителей найденных объектов'),'req'=>0)
),'group'=>GROUP_TBL_FILTER,'pos'=>0,'result'=>CMD_FIND_COUNT);


$cmd['where'][CMD_FIND_COUNT]->visual=Array('title'=>'Условие','put'=>Array(
		0=>Array('title'=>'Поле','type'=>STRING_COLNAME_FIND,'req'=>1),
		1=>Array('title'=>'Операция','type'=>CMD_STRING,'list'=>Array(
			"'=='"=>'==',
			"'!='"=>'!=',
			"'<'"=>'<',
			"'<='"=>'<=',
			"'>'"=>'>',
			"'>='"=>'>=',
			'contain'=>'поле содержит значение',
			'not_contain'=>'поле не содержит значения',
			'contained'=>'значение содержит поле',
			'not_contained'=>'значение не содержит поля',
			'in_array'=>'поле содержится в массиве',
			'not_in_array'=>'поле не содержится в массиве',
			'empty'=>'пустое',
			'not_empty'=>'не пустое'
		),'req'=>1),
		2=>Array('title'=>'Значение','type'=>CMD_STRING,'req'=>0),
		3=>Array('title'=>'Мутатор поля','type'=>CMD_STRING,'list'=>Array(''=>'Нет','mutate_lower'=>'В нижнем регистре','mutate_length'=>'Работать с длинной поля'),'req'=>0),
),'pos'=>2,'result'=>CMD_FIND_COUNT);


$cmd['where_set'][CMD_FIND_COUNT]->visual=Array('title'=>'Набор условий','put'=>Array(
		0=>Array('title'=>'Тип проверки условий','type'=>CMD_STRING,'list'=>Array('and'=>'и','or'=>'или'),'req'=>1),
		1=>Array('title'=>'Условия','type'=>-CMD_FIND_WHERE,'is_array'=>1,'req'=>1)
),'pos'=>3,'result'=>CMD_FIND_COUNT);


$cmd['sort'][CMD_FIND_COUNT]->visual=Array('title'=>'Сортировать','put'=>Array(
		0=>Array('title'=>'Поле для сортировки','type'=>STRING_COLNAME_FIND,'req'=>1),
		1=>Array('title'=>'Тип сортировки','type'=>CMD_STRING,'list'=>Array(''=>'По возрастанию','desc'=>'По убыванию','rand'=>'Случайным образом'),'req'=>0),
),'pos'=>4,'result'=>CMD_FIND_COUNT);


$cmd['start'][CMD_FIND_COUNT]->visual=Array('title'=>'Смещение','put'=>Array(
		0=>Array('title'=>'Пропустить объектов','type'=>CMD_STRING,'req'=>1),
),'pos'=>5,'result'=>CMD_FIND_COUNT);


$cmd['limit'][CMD_FIND_COUNT]->visual=Array('title'=>'Лимит','put'=>Array(
		0=>Array('title'=>'Лимит объектов','type'=>CMD_STRING,'req'=>1),
),'pos'=>6,'result'=>CMD_FIND_COUNT);


$cmd['do'][CMD_FIND_COUNT]->visual=Array('title'=>'Вернуть результат','pos'=>1000,'special'=>SP_IGNORE,'result'=>/*STRING_NUM*/CMD_STRING);


$cmd[''][CMD_TABLE]->process=function(&$data,&$op,&$ops){
		$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $row=$t['row']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
		global $filter;
		if(isset($filter[$op->data][$table][$ex][$owner])) return $filter[$op->data][$table][$ex][$owner];
};
$cmd[''][CMD_TABLE]->result=CMD_ARRAY;
$cmd[''][CMD_TABLE]->result_long=CMD_ROW;
$cmd[''][CMD_TABLE]->visual=Array('title'=>'','special'=>SP_UNKNOWN);

?>