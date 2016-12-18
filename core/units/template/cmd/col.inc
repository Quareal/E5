<?php

function seek_col($cid){
	global $pccol,$db,$pccol2;
	if(is_object($cid) && isset($cid->row)) $row=$cid->row; else $row=0;
	if(empty($row) && is_object($cid) && isset($cid->own)) $row=$cid->own;
	if(empty($row) && !empty($GLOBALS['cur_row'])) $row=$GLOBALS['cur_row'];//?
	if(empty($row) && !empty($GLOBALS['current'])) $row=$GLOBALS['current'];//?
	if(is_object($cid) && isset($cid->id) && !is_numeric($cid->id)) $cid=$cid->id;
	if(is_string($cid) && !is_numeric($cid)){
		$col_sname=$cid;
		$pc2=&$pccol2[$GLOBALS["cur_module"]][$GLOBALS["cur_table"]][$col_sname];
		if(!empty($GLOBALS["cur_table"])){
			if(!isset($pc2)){
				getrow($db,"SELECT * FROM main_col WHERE col_table=".$GLOBALS["cur_table"]." AND (col_sname='$col_sname')",1,'main_col',__LINE__,__FILE__);
				if(!empty($db->Record["col_id"])){
					$pccol[$db->Record["col_id"]]=$db->Record;
					$pc2=$db->Record;
				}
			} else $db->Record=$pc2;
		} else {
			if(empty($pc2)){
				getrow($db,"SELECT * FROM main_col WHERE col_table=0 AND col_module=".$GLOBALS["cur_module"]." AND (col_sname='$col_sname')",1,'main_col',__LINE__,__FILE__);
				$pccol[$db->Record["col_id"]]=$db->Record;
				$pc2=$db->Record;
			} else $db->Record=$pc2;
		}
		if(isset($pc2)) $cid=$pc2["col_id"]; else $cid=0;
	}
	if(is_object($cid) && isset($cid->id)) $cid=$cid->id;
	if(!isset($pccol[$cid])){
		if(empty($cid)) return false;
		getrow($db,"SELECT * FROM main_col WHERE col_id=$cid",1,'main_col',__LINE__,__FILE__);
		$pccol[$cid]=$db->Record;
	}
	return Array('col'=>$pccol[$cid], 'row'=>$row, 'id'=>$cid);
}


$cmd['id'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		return $c["col_id"].':'.$row.':'.$c["col_table"].':'.$c["col_module"].':'.$c["col_sname"];
};
$cmd['id'][CMD_COL]->result=CMD_STRING;


$cmd['value'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		global $prowval,$db;
		$ccol=$c;
		$col_id=$c["col_id"];
		$col_link2=$c["col_link2"];
		$col_type=$c["col_type"];
		$prow=&$prowval[$col_id][$crow];
		if(isset($GLOBALS["spec_step2"])) $GLOBALS["tree_vars"][$GLOBALS["spec_step2"]][$col_id]=$c;
		if(empty($prow)){
			global $vcache;
			if(isset($vcache[$col_id][$crow])){
				$res=Array();
				if(!empty($vcache[$col_id][$crow]) && is_array($vcache[$col_id][$crow])) foreach($vcache[$col_id][$crow] AS $v) $res[]=$v;
			} else {
				$res=getall($db,"SELECT * FROM row_value WHERE value_col=".$col_id." AND value_row=".$crow,1,'row_value',__LINE__,__FILE__);
				$vcache2[$col_id][$crow]='';$fir=true;
				if(!empty($res)) foreach($res AS $v){
					if(!empty($ccol["col_force_onshow"])) $v["value_value"]=force_value($v,$ccol);
					if(isset($vcache[$col_id]) && !is_array($vcache[$col_id])) $vcache[$col_id]=Array();
					$vcache[$col_id][$crow][$v["value_id"]]=$v;
					if($fir) $vcache2[$col_id][$crow]=$v;
					$fir=false;
				}
			}
			$prow=$res;
		} else $res=$prow;
		if(!empty($res)){
			if(count($res)==1 || $col_type==0 || ($col_link2==0 && ($col_type==1 || $col_type==5 || $col_type==7 || $col_type==4))){
				//выше col_type стоит для редких случаев, когда у одного row оказывается два value по одному и тому же текстовому col (а вот почему так происходит - загадка)
				if($col_type==1){
					return prepend_type($res[0]["value_value"],'row');
				} else if($col_type==5){
					return prepend_type($res[0]["value_value"],'usr');
				} else {
					return $res[0]["value_value"];
				}
			} else{
				$res2=Array();
				foreach($res AS $r){
					global $rlink;
					seek_rlink($r["value_value"]);
					$res2[]->id=$r["value_value"];
				}
				return $res2;
			}
		}
		return false;
};
$cmd['value'][CMD_COL]->result=CMD_MIXED;
$cmd['value'][CMD_COL]->visual=Array('title'=>'значение');


$cmd['set'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		global $db, $rlink;
		if(empty($crow)) return false;
		$row=$crow;
		seek_rlink($row);
		$use_handler=($op->data=='set_with_handler');
		if($c["col_type"]!=3){
			$data=prepend_value(parse_var($op->put/*[0]*/));
			clear_rowval_cache($c["col_id"],$row);
			$db->query('DELETE FROM row_value WHERE value_row='.$row.' AND value_col='.$c["col_id"].' AND value_table>0',3,'row_value',__LINE__,__FILE__);			
			if(is_array($data) && $c['col_type']==1 && $c['col_link2']==1){
				foreach($data AS $key=>$value)if(!empty($value)){
					if(is_object($value) && isset($value->id)) $value=$value->id;
					$db->query('INSERT INTO row_value (value_module,value_table,value_col,value_row,value_value)
						VALUES ('.$rlink[$row]->module.','.$rlink[$row]->table.','.$c["col_id"].','.$row.',\''.$value.'\')',3,'row_value',__LINE__,__FILE__);
				}
			} else {
				$db->query('INSERT INTO row_value (value_module,value_table,value_col,value_row,value_value)
					VALUES ('.$rlink[$row]->module.','.$rlink[$row]->table.','.$c["col_id"].','.$row.',\''.$data.'\')',3,'row_value',__LINE__,__FILE__);
			}
			rebuild_row_index($row,$c["col_id"]);
			//return false;
		} else {
			global $cmd;
			//$x=&$cmd['set'][CMD_COL_FILE]->process;
			$x=get_cmd_link('set',CMD_COL_FILE);
			//return $x($data,$op,$ops);
			$x($data,$op,$ops);
			//return false;
		}
		if($use_handler) on_col_edit($row,$c['col_id'],$c['col_oninsert'],$rlink[$row]->module,get_ex($row,$rlink[$row]->table,$rlink[$row]->tex),$rlink[$row]->table);
		return false;
};
$cmd['set'][CMD_COL]->result=CMD_NONE;
$cmd['set'][CMD_COL]->visual=Array('title'=>'Установить значение','put'=>Array(
		0=>Array('title'=>'значение','type'=>CMD_COL_TYPE,'req'=>1),
		1=>Array('title'=>'расширение файла (для файлов)','type'=>CMD_STRING,'req'=>0)
));
$cmd['set_with_handler'][CMD_COL]->alias=&$cmd['set'][CMD_COL];
$cmd['set_with_handler'][CMD_COL]->visual=Array('title'=>'Установить значение (с обработчиком)','put'=>Array(
		0=>Array('title'=>'значение','type'=>CMD_COL_TYPE,'req'=>1),
		1=>Array('title'=>'расширение файла (для файлов)','type'=>CMD_STRING,'req'=>0)
));


$cmd['file'][CMD_COL]->process=function(&$data,&$op,&$ops){
		return $data;
};
$cmd['file'][CMD_COL]->result=CMD_COL_FILE;
$cmd['file'][CMD_COL]->visual=Array('title'=>'операции с файлами','condition'=>IF_COL_FILE);


$cmd['set'][CMD_COL_FILE]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		global $db;
		if(empty($row) || $c["col_type"]!=3) return false;
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		if(isset($op->put[1])) $ext=parse_var($op->put[1]); else $ext='';
		$data=parse_var($op->put[0]);
		set_col_file(/*$fname*/$data,$row,$c);
		return false;
};
$cmd['set'][CMD_COL_FILE]->result=CMD_NONE;
$cmd['set'][CMD_COL_FILE]->visual=Array('title'=>'установить новый файл','put'=>Array(
		0=>Array('title'=>'Путь до нового файла','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Расширение файла','type'=>CMD_STRING,'req'=>0)
));


$cmd['clone'][CMD_COL_FILE]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		if(empty($row) || $c["col_type"]!=3) return false;
		$res=clone_col_file($c,$row);
		return $res;
};
$cmd['clone'][CMD_COL_FILE]->result=CMD_STRING;
$cmd['clone'][CMD_COL_FILE]->visual=Array('title'=>'копировать файл');


$cmd['rename'][CMD_COL_FILE]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		if(empty($row) || $c["col_type"]!=3) return false;
		return new_rand_name($row,$col["col_id"]);
};
$cmd['rename'][CMD_COL_FILE]->result=CMD_STRING;
$cmd['rename'][CMD_COL_FILE]->visual=Array('title'=>'переименовать файл');


$cmd['size'][CMD_COL_FILE]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		if(empty($row) || $c["col_type"]!=3) return false;
		$res=rowcol_val($row,$col);
		if(file_exists(DOCUMENT_ROOT.$res)) $res=filesize(DOCUMENT_ROOT.$res);
		else $res=0;
		return $res;
};
$cmd['size'][CMD_COL_FILE]->result=CMD_STRING;
$cmd['filesize'][CMD_COL_FILE]->alias=&$cmd['size'][CMD_COL_FILE];
$cmd['size'][CMD_COL_FILE]->visual=Array('title'=>'размер файла');


$cmd['get'][CMD_COL_FILE]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		if(empty($row) || $c["col_type"]!=3) return false;
		$res=rowcol_val($row,$col);
		if($res=='') return '';
		if(file_exists(DOCUMENT_ROOT.$res)) $res=file_get_contents2(DOCUMENT_ROOT.$res);
		else $res='';
		return $res;
};
$cmd['get'][CMD_COL_FILE]->result=CMD_STRING;
$cmd['getfile'][CMD_COL_FILE]->alias=&$cmd['get'][CMD_COL_FILE];
$cmd['get'][CMD_COL_FILE]->visual=Array('title'=>'содержимое файла');


$cmd['formname'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		if(!empty($GLOBALS["glob_prefix"])) return $GLOBALS["glob_prefix"].'col'.$c["col_id"];
		else return 'col'.$c["col_id"];
};
$cmd['formname'][CMD_COL]->result=CMD_STRING;
$cmd['formname'][CMD_COL]->visual=Array('title'=>'имя для элемента формы');


$cmd['name'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		return $c["col_name"];
};
$cmd['name'][CMD_COL]->result=CMD_STRING;
$cmd['name'][CMD_COL]->visual=Array('title'=>'имя');


$cmd['sname'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		return $c["col_sname"];
};
$cmd['sname'][CMD_COL]->result=CMD_STRING;
$cmd['sname'][CMD_COL]->visual=Array('title'=>'спец.имя');


$cmd['table'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		global $rlink;
		seek_rlink($row);
		$ex=0;
		if(!empty($rlink[$row])){
			$ex=get_ex2($rlink[$row]->tex);
		}
		return prepend_type($c['col_table'],'tbl',Array('row'=>$row,'ex'=>$ex));
};
$cmd['table'][CMD_COL]->result=CMD_TABLE;
$cmd['table'][CMD_COL]->visual=Array('title'=>'таблица');


$cmd['folder'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		return $c["file_dir"];
};
$cmd['folder'][CMD_COL]->result=CMD_STRING;
$cmd['folder'][CMD_COL]->visual=Array('title'=>'папка','condition'=>IF_COL_FILE);


$cmd['prefix'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		return $c["file_prefix"];
};
$cmd['prefix'][CMD_COL]->result=CMD_STRING;
$cmd['prefix'][CMD_COL]->visual=Array('title'=>'префикс файла','condition'=>IF_COL_FILE);


$cmd['form'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		if(isset($op->put)) $val=parse_var($op->put); else $val='';
		return show_form($c,$c["col_table"],$row,0,$val);
};
$cmd['form'][CMD_COL]->result=CMD_STRING;
$cmd['form'][CMD_COL]->visual=Array('title'=>'обработчик формы','put'=>Array(
		0=>Array('title'=>'значение для компонента','type'=>CMD_MIXED,'req'=>0)
));


$cmd['form_noprocess'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		if(isset($op->put)) $val=parse_var($op->put); else $val='';
		return show_form($c,$c["col_table"],$row,1,$val);
};
$cmd['form_noprocess'][CMD_COL]->result=CMD_STRING;
$cmd['form2'][CMD_COL]->alias=&$cmd['form_noprocess'][CMD_COL];
$cmd['form_noprocess'][CMD_COL]->visual=Array('title'=>'стандартный компонент формы','put'=>Array(
		0=>Array('title'=>'значение для компонента','type'=>CMD_MIXED,'req'=>0)
));


$cmd['show'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		global $f_type;
		$oldft=$f_type;
		$f_type='onshow';
		$r=backup_globals();
		$GLOBALS["cur_table"]=$c["col_table"];
		$GLOBALS["cur_row"]=$row;
		unset($GLOBALS["cur_col"]);
		$GLOBALS["cur_col"]->id=$col_id;
		$GLOBALS["cur_col"]->row=$row;
		$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$GLOBALS["cur_row"];
		$res=shell_tpl($col["col_onshow"]);
		return_globals($r);
		$f_type=$oldft;
		return $res;
};
$cmd['show'][CMD_COL]->result=CMD_STRING;
$cmd['show'][CMD_COL]->visual=Array('title'=>'использовать компонент вывода');


$cmd['type'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		return $c["col_type"];
};
$cmd['type'][CMD_COL]->result=CMD_STRING;
$cmd['type'][CMD_COL]->visual=Array('title'=>'тип поля');


$cmd['is_tpl'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		return $c["col_tpl"];
};
$cmd['is_tpl'][CMD_COL]->result=CMD_STRING;
$cmd['is_tpl'][CMD_COL]->visual=Array('title'=>'поле может содержать шаблон');


$cmd['is_url'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		return $c["col_url"];
};
$cmd['is_url'][CMD_COL]->result=CMD_STRING;
$cmd['is_url'][CMD_COL]->visual=Array('title'=>'поле предназначено для разбора URL');


$cmd['is_major'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		global $major_col_cache;
		$table=$c['col_table'];
		if(!$c['col_table']) return false;
		if(empty($major_col_cache[$table])) $major_col_cache[$table]=getrowval("SELECT major_col FROM main_table WHERE table_id=".$c['col_table'],'major_col');
		return ($major_col_cache[$table]==$col_id);
};
$cmd['is_major'][CMD_COL]->result=CMD_STRING;
$cmd['is_major'][CMD_COL]->visual=Array('title'=>'поле является главным');


$cmd['row'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		return prepend_type($row,'row',Array('table'=>$c["col_table"]));
};
$cmd['row'][CMD_COL]->result=CMD_ROW;
$cmd['row'][CMD_COL]->visual=Array('title'=>'объект');


$cmd['linked'][CMD_COL]->process=function(&$data,&$op,&$ops){
		$cd=seek_col($data); $c=$cd["col"]; if(empty($c)) return false; $col_id=$cd["id"]; $col_sname=$c["col_sname"]; $row=$cd["row"]; $crow=$row;
		if(!empty($row)){
			global $rlink;
			seek_rlink($row);			
			return prepend_type($c["col_link"],'table',Array('ex'=>get_ex2($rlink[$row]->tex)));
		} else return prepend_type($c["col_link"],'table');
};
$cmd['linked'][CMD_COL]->result=CMD_TABLE;
$cmd['linked'][CMD_COL]->visual=Array('title'=>'Связная таблица');


?>