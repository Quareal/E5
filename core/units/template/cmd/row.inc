<?php

define('GROUP_ROW_SPEC',4001);
$cmd_group[GROUP_ROW_SPEC]=Array('title'=>'Специфические','pos'=>20);
define('GROUP_ROW_OPS',4002);
$cmd_group[GROUP_ROW_OPS]=Array('title'=>'Операции','pos'=>2);
define('GROUP_ROW_ACC',4003);
$cmd_group[GROUP_ROW_ACC]=Array('title'=>'Доступы','pos'=>5);
define('GROUP_ROW_NAV',4004);
$cmd_group[GROUP_ROW_NAV]=Array('title'=>'Навигация','pos'=>3);
define('GROUP_ROW_URL',4005);
$cmd_group[GROUP_ROW_URL]=Array('title'=>'URL','pos'=>4);
define('GROUP_ROW_SEARCH',4006);
$cmd_group[GROUP_ROW_SEARCH]=Array('title'=>'Поиск','pos'=>6);
define('GROUP_ROW_ARR',4007);
$cmd_group[GROUP_ROW_ARR]=Array('title'=>'Массивы','pos'=>6);
define('GROUP_ROW_PARAM',4008);
$cmd_group[GROUP_ROW_PARAM]=Array('title'=>'Параметры','pos'=>2);


$cmd['_function'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		$type=$op->data;
		$op=array_shiftA($ops);
		return parse_component($type,$op,$data);
};
$cmd['_function'][CMD_ROW]->result=CMD_MIXED;
$cmd['_func'][CMD_ROW]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_component'][CMD_ROW]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_show'][CMD_ROW]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_form'][CMD_ROW]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_part'][CMD_ROW]->alias=&$cmd['_function'][CMD_ROW];

/*$cmd['_function'][CMD_ROW]->visual=Array('title'=>'функции','put'=>Array(0=>Array('title'=>'функция','type'=>STRING_FUNCTION_NAME,'req'=>1)), 'special'=>SP_POINT,'extend_put'=>1);
$cmd['_show'][CMD_ROW]->visual=Array('title'=>'отображения','put'=>Array(0=>Array('title'=>'отображение','type'=>STRING_SHOW_NAME,'req'=>1)), 'special'=>SP_POINT,'extend_put'=>1);
$cmd['_component'][CMD_ROW]->visual=Array('title'=>'компоненты','put'=>Array(0=>Array('title'=>'компонент','type'=>STRING_COMPONENT_NAME,'req'=>1)), 'special'=>SP_POINT,'extend_put'=>1);
$cmd['_form'][CMD_ROW]->visual=Array('title'=>'формы','put'=>Array(0=>Array('title'=>'форма','type'=>STRING_FORM_NAME,'req'=>1)), 'special'=>SP_POINT,'extend_put'=>1);*/

$cmd['_function'][CMD_ROW]->visual=Array('title'=>'функции', 'special'=>SP_COMPONENT,'extend_put'=>1,'group'=>GROUP_ROW_SPEC);
$cmd['_show'][CMD_ROW]->visual=Array('title'=>'отображения', 'special'=>SP_COMPONENT,'extend_put'=>1,'group'=>GROUP_ROW_SPEC);
$cmd['_component'][CMD_ROW]->visual=Array('title'=>'компоненты', 'special'=>SP_COMPONENT,'extend_put'=>1,'group'=>GROUP_ROW_SPEC);
$cmd['_form'][CMD_ROW]->visual=Array('title'=>'формы', 'special'=>SP_COMPONENT,'extend_put'=>1,'group'=>GROUP_ROW_SPEC);

$cmd['_part'][CMD_ROW]->visual=Array('title'=>'части', 'special'=>SP_PART,'extend_put'=>1,'group'=>GROUP_ROW_SPEC);

$cmd['highlight'][CMD_ROW]->alias=&$cmd['highlight'][CMD_STRING];
$cmd['highlight_only'][CMD_ROW]->alias=&$cmd['highlight'][CMD_STRING];
$cmd['highlight_only'][CMD_ROW]->visual=Array('title'=>'семантический поиск (только подсветка)', 'put'=>Array(
		0=>Array('title'=>'Запрос','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'CSS класс выделенных слов','type'=>CMD_STRING,'req'=>0),
		2=>Array('title'=>'CSS стили выделенных элементов','type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_ROW_SEARCH);
$cmd['highlight'][CMD_ROW]->visual=Array('title'=>'семантический поиск', 'put'=>Array(
		0=>Array('title'=>'Запрос','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'CSS класс выделенных слов','type'=>CMD_STRING,'req'=>0),
		2=>Array('title'=>'CSS стили выделенных элементов','type'=>CMD_STRING,'req'=>0),
		3=>Array('title'=>'Количество слов справа и слева от искомого','type'=>CMD_STRING,'req'=>0),
		4=>Array('title'=>'Максимальное количество выданных слов','type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_ROW_SEARCH);
$cmd['highlight2'][CMD_ROW]->alias=&$cmd['highlight'][CMD_STRING];


$cmd['copy'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $db;
		if(!empty($op->put)){
			$ownr=parse_var($op->put);
			$new_owner=true;
		} else {
			$new_owner=false;
		}
		$single=($op->data=='copy_single');
		if(!$new_owner || !isset($ownr)) $res=copy_row($row,$rlink[$row]->rid,$rlink[$row]->owner,$rlink[$row]->tex,$rlink[$row]->module,$rlink[$row]->table,$rlink[$row]->ownert,$rlink[$row]->module,$rlink[$row]->rsub,!$single);
		else {
			if(strstr($ownr->id,':')){
				$ownr->id=explode(':',$ownr->id);
				$ownr->id=$ownr->id[0];
			}
			if(!isset($ownr->tex)){
				$x=get_rlink($ownr->id);
				$ownr->tex=$x->tex;
			}
			if(!isset($ownr->owner) && isset($ownr->own)) $ownr->owner=$ownr->own;
			$ownr_tbl=$ownr->id;
			$rsub=0;
			if($ownr->owner){
				seek_rlink($ownr->owner);
				if($rlink[$ownr->owner]->table!=$ownr->id){
					$ownr_tbl=$rlink[$ownr->owner]->table;
					$rsub=$rlink[$ownr->owner]->id;
				} else $rsub=$rlink[$ownr->owner]->rsub;
			}			
			$new_mod=getrowval("SELECT table_id, table_module FROM main_table WHERE table_id=$ownr->id","table_module");
			$res=copy_row($row,$rlink[$row]->rid,$ownr->owner,$ownr->tex,$new_mod,$ownr->id,$ownr_tbl,$new_mod,$rsub,!$single);
		}
		seek_rlink($res);
		$res=$rlink[$res];
		return $res;
};
$cmd['copy'][CMD_ROW]->result=CMD_ROW;
$cmd['copy_single'][CMD_ROW]->alias=&$cmd['copy'][CMD_ROW];
$cmd['copy'][CMD_ROW]->visual=Array('title'=>'копировать','put'=>Array(
		0=>Array('title'=>'таблица, куда будет копироваться объект','type'=>CMD_TABLE,'req'=>1)
),'group'=>GROUP_ROW_OPS);
$cmd['copy_single'][CMD_ROW]->visual=Array('title'=>'копировать (без потомков)','put'=>Array(
		0=>Array('title'=>'таблица, куда будет копироваться объект','type'=>CMD_TABLE,'req'=>1)
),'group'=>GROUP_ROW_OPS);


$cmd['enable?'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		$tmp=$rlink[$row]->enable;
		return $tmp;
};
$cmd['enable?'][CMD_ROW]->result=CMD_STRING;
$cmd['enable?'][CMD_ROW]->visual=Array('title'=>'объект активен?','group'=>GROUP_ROW_PARAM);


$cmd['pos'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		return $rlink[$row]->pos;
};
$cmd['pos'][CMD_ROW]->result=CMD_STRING;
$cmd['pos'][CMD_ROW]->visual=Array('title'=>'позиция','group'=>GROUP_ROW_PARAM);


$cmd['self'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		return $rlink[$row];
};
$cmd['self'][CMD_ROW]->result=CMD_ROW;


$cmd['admin_url'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		return get_admin_url($row);
};
$cmd['admin_url'][CMD_ROW]->result=CMD_STRING;
$cmd['aurl'][CMD_ROW]->alias=&$cmd['admin_url'][CMD_ROW];
$cmd['admin_url'][CMD_ROW]->visual=Array('title'=>'Ссылка на редактирование в кабинете администратора','group'=>GROUP_ROW_URL);


$cmd['full_url'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		return get_row_url($row);
};
$cmd['full_url'][CMD_ROW]->result=CMD_STRING;
$cmd['full_url'][CMD_ROW]->visual=Array('title'=>'Путь до объекта на сайте','group'=>GROUP_ROW_URL);


$cmd['enable'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $db;
		if(check_row($row,$rlink[$row]->table,get_ex2($rlink[$row]->tex),'edit',$rlink[$row]->user,$rlink[$row]->users,$rlink[$row]->module/*тут сомненья, т.к. не ясно какой модуль требуется для проверки - ведь у подтаблицы и надтаблицы могут быть разные модули*/)){
			$db->query("UPDATE main_row SET row_enable=1 WHERE row_id=$row",3,'main_row',__LINE__,__FILE__);
			$db->query("UPDATE row_owner SET ro_enable=1 WHERE row_id=$row",3,'row_owner',__LINE__,__FILE__);
			del_cache('row',$row);
		}
};
$cmd['enable'][CMD_ROW]->result=CMD_NONE;
$cmd['enable'][CMD_ROW]->visual=Array('title'=>'активировать','group'=>GROUP_ROW_OPS);


$cmd['disable'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $db;
		if(check_row($row,$rlink[$row]->table,get_ex2($rlink[$row]->tex),'edit',$rlink[$row]->user,$rlink[$row]->users,$rlink[$row]->module/*тут сомненья, т.к. не ясно какой модуль требуется для проверки - ведь у подтаблицы и надтаблицы могут быть разные модули*/)){	
			$db->query("UPDATE main_row SET row_enable=0 WHERE row_id=$row",3,'main_row',__LINE__,__FILE__);
			$db->query("UPDATE row_owner SET ro_enable=0 WHERE row_id=$row",3,'row_owner',__LINE__,__FILE__);
			del_cache('row',$row);
		}		
};
$cmd['disable'][CMD_ROW]->result=CMD_NONE;
$cmd['disable'][CMD_ROW]->visual=Array('title'=>'деактивировать','group'=>GROUP_ROW_OPS);


$cmd['edit'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;

		global $prepost;
		if(!empty($op->put)){
			check_single($op->put,$GLOBALS["lex_func_sep"]);
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
		
		$c=find_cmd('editrow',CMD_BASE,$row);
		$tmp=do_cmd($op,$ops,$c,$row);		
		
		return false;
};
$cmd['edit'][CMD_ROW]->result=CMD_NONE;
$cmd['edit'][CMD_ROW]->visual=Array('title'=>'изменить','group'=>GROUP_ROW_OPS,'special'=>SP_ADD_COLS);
$cmd['editrow'][CMD_ROW]->alias=&$cmd['edit'][CMD_ROW];


$cmd['table'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		if(empty($row)) return false;
		$owner=$rlink[$row]->owner;
		$ex=get_ex($row,$table);
		return prepend_type($table,'tbl',Array('ex'=>$ex, /*'own'=>$owner,'row'=>$row*/'row'/*own*/=>$row));
};
$cmd['table'][CMD_ROW]->result=CMD_TABLE;
$cmd['table'][CMD_ROW]->visual=Array('title'=>'таблица','group'=>GROUP_ROW_PARAM);


$cmd['prepare'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $cmd;		
		$x=get_cmd_link('prepare',CMD_ARRAY);
		$y=Array($r);
		return $x($y,$op,$ops);
};
$cmd['prepare'][CMD_ROW]->result=CMD_MIXED; //ROW or ARRAY
$cmd['prepare'][CMD_ROW]->visual=Array('title'=>'подгрузка','special'=>SP_PREPARE,'group'=>GROUP_ROW_SPEC);


$cmd['id'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		$ex=get_ex($row,$table);
		$owner=$row;
		$db->Record=table_cache($table);
		$stable=$db->Record["table_sname"];
		$res=$table.':'.$ex.':'.$owner.':'.$stable.':'.get_tex($owner,$ex,$table);
		return $res;
};
$cmd['id'][CMD_ROW]->result=CMD_STRING;


$cmd['user'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		$u=$rlink[$row]->user;
		return prepend_type($u,'usr');
};
$cmd['user'][CMD_ROW]->result=CMD_USER;
$cmd['user'][CMD_ROW]->visual=Array('title'=>'владелец','group'=>GROUP_ROW_PARAM);


$cmd['linked'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $db,$linked_count_cache;
		$sql_add='';
		$sql_add2='';
		if(!empty($op->put)){
			$tableA=parse_var($op->put);
		} else $tableA='';
		if($op->data=='linked_count'){
			$seek_count=1;
			if(isset($linked_count_cache[$row.'-'.serialize($tableA)])) return $linked_count_cache[$row.'-'.serialize($tableA)];
		} else $seek_count=0;
		if(!empty($op->put)){
			if(is_object($tableA) && isset($tableA->id)) $tableA=$tableA->id;
			if(is_string($tableA) && strstr($tableA,':')){
				$tmp=explode(':',$tableA);
				$tableA=$tmp[0];
			}
			if(!is_numeric($tableA)){
				$tableA=getall3($db,"SELECT table_id,table_sname FROM main_table WHERE table_sname='$tableA'","table_id");
				$sql_add=' AND value_table IN ('.implode(',',$tableA).')';
				$sql_add2=' AND col_table IN ('.implode(',',$tableA).')';
			} else {
				$sql_add=' AND value_table='.$tableA;
				$sql_add2=' AND col_table='.$tableA;
			}
		} else $tableA='';
		if($op->data=='linked_same_module' && !empty($rlink[$row]->module)) $sql_add.=" AND value_module=".$rlink[$row]->module;
		if(empty($tableA)) $cols=getall3($db,"SELECT col_id, col_type, col_link FROM main_col WHERE col_table>0 AND col_type=1 AND col_link=$table","col_id");
		//условие col_table>0 исключает из выдачи экземпляры модуля
		if(empty($cols) && !empty($tableA)) $cols=getall3($db,"SELECT col_id, col_type, col_link FROM main_col WHERE col_table>0 AND col_type=1 AND col_link>0".$sql_add2,"col_id");
		if(!empty($cols)){
			if($seek_count){
				$count=getrowval("SELECT count(*) AS count FROM row_value WHERE value_value='$row' AND value_col IN (".implode(',',$cols).")".$sql_add,"count");
				$linked_count_cache[$row.'-'.serialize($tableA)]=$count;
				return $count;
			} else {
				$nums=getall3($db,"SELECT * FROM row_value WHERE value_value='$row' AND value_col IN (".implode(',',$cols).")".$sql_add,"value_row");
			}
			$nums=deep_seek_activity($nums);
			$res=nums_to_rows($nums);
		} else $res=Array();
		if($op->data=='linked_same_ex'){
			$tmp=$res;
			$res=Array();
			if(!empty($rlink[$row]->tex)){
				$exs=get_ex_all($rlink[$row]->tex);
				if(!empty($exs)){
					foreach($tmp AS $tm) if(!empty($tm->tex)){
						$c_exs=get_ex_all($tm->tex);
						if(!empty($c_exs)) foreach($c_exs AS $c_ex) if(!empty($exs[$c_ex])){
							$res[count($res)]=$tm;
							break;
						}
					}
				}
			}
		}
		return $res;
};
$cmd['linked'][CMD_ROW]->result=CMD_ARRAY;
$cmd['linked'][CMD_ROW]->result_long=CMD_ROW;
$cmd['linked_same_module'][CMD_ROW]->alias=&$cmd['linked'][CMD_ROW];
$cmd['linked_same_ex'][CMD_ROW]->alias=&$cmd['linked'][CMD_ROW];
$cmd['linked'][CMD_ROW]->visual=Array('title'=>'связанные объекты','put'=>Array(
		0=>Array('title'=>'Таблица','type'=>STRING_TABLE_NAME_ID,'in_quotes'=>1,'req'=>0)
),'group'=>GROUP_ROW_ARR);
$cmd['linked_same_module'][CMD_ROW]->visual=Array('title'=>'связанные объекты из текущего модуля','put'=>Array(
		0=>Array('title'=>'Таблица','type'=>STRING_TABLE_NAME_ID,'in_quotes'=>1,'req'=>0)
),'group'=>GROUP_ROW_ARR);
$cmd['linked_same_ex'][CMD_ROW]->visual=Array('title'=>'связанные объекты из текущего экземпляра','put'=>Array(
		0=>Array('title'=>'Таблица','type'=>STRING_TABLE_NAME_ID,'in_quotes'=>1,'req'=>0)
),'group'=>GROUP_ROW_ARR);

$cmd['linked_count'][CMD_ROW]->alias=&$cmd['linked'][CMD_ROW];
$cmd['linked_count'][CMD_ROW]->result=CMD_STRING;
$cmd['linked_count'][CMD_ROW]->visual=Array('title'=>'Количество связанных объектов','put'=>Array(
		0=>Array('title'=>'Таблица','type'=>STRING_TABLE_NAME_ID,'in_quotes'=>1,'req'=>0)
),'group'=>GROUP_ROW_ARR);


$cmd['signal'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$tmp[0]=$op->put[0];
		if(empty($op->put[1])) $tmp[1]=0;
		else $tmp[1]=$op->put[1];
		add_news($tmp[0],'',$tmp[1],$row);
};
$cmd['signal'][CMD_ROW]->result=CMD_NONE;
$cmd['signal'][CMD_ROW]->visual=Array('title'=>'Оповещение','put'=>Array(
		0=>Array('title'=>'Сообщение','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Пользователь или группа','type'=>CMD_USER_GROUP,'req'=>0)
),'group'=>GROUP_ROW_SPEC);


$cmd['set_user'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $db;
		$new_user=parse_var($op->put);
		$ex=get_ex($row,$table);	
		$ex2=get_tex($row,$ex,$table);
		$ex_table2=$ex2;
		$ex_ex2v=$ex;
		if(check_row($row,$ex_table2,$ex_ex2v,'edit',$rlink[$row]->user,$rlink[$row]->users)){
			$db->query("UPDATE main_row SET row_user='.$new_user.' WHERE row_id=$row",3,"main_row");
			$db->query("UPDATE row_owner SET ro_user='.$new_user.' WHERE row_id=$row",3,"row_owner");
		}
		del_cache('row',$row);
};
$cmd['set_user'][CMD_ROW]->result=CMD_NONE;
$cmd['set_user'][CMD_ROW]->visual=Array('title'=>'Сменить владельца','put'=>Array(
		0=>Array('title'=>'новый владелец','type'=>CMD_USER,'req'=>1)
),'group'=>GROUP_ROW_OPS);


$cmd['next'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		if(isset($rlink[$row]->next)) $oid=$rlink[$row]->next->id;
		else {
			$ex=get_ex($row,$table);	
			$ex2=get_tex($row,$ex,$table);
			$seek_ex=1;
			if(!empty($rlink[$row]->owner)) $seek_ex=0;
			get_sub($rlink[$row]->owner,$table,1,$seek_ex,0,0,0,$ex2,$ex,$table);
			if(isset($rlink[$row]->next)) $oid=$rlink[$row]->next->id; else return false;
		}
		return prepend_type($oid,'row');
};
$cmd['next'][CMD_ROW]->result=CMD_ROW;
$cmd['next'][CMD_ROW]->visual=Array('title'=>'Следующий объект','group'=>GROUP_ROW_NAV);


$cmd['prev'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		if(isset($rlink[$row]->prev)) $oid=$rlink[$row]->prev->id;
		else {
			$ex=get_ex($row,$table);	
			$ex2=get_tex($row,$ex,$table);
			get_sub($rlink[$row]->owner,$table,1,0,0,0,0,$ex2,$ex,$table);
			if(isset($rlink[$row]->prev))  $oid=$rlink[$row]->prev->id; else return '';
		}
		return prepend_type($oid,'row');
};
$cmd['prev'][CMD_ROW]->result=CMD_ROW;
$cmd['prev'][CMD_ROW]->visual=Array('title'=>'Предыдущий объект','group'=>GROUP_ROW_NAV);


$cmd['next_simple'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $db;
		getrow($db,"SELECT * FROM row_owner WHERE row_table=".$rlink[$row]->table." AND ro_ex=".$rlink[$row]->tex." AND owner_id=".$rlink[$row]->owner." AND ro_pos>".$rlink[$row]->pos." AND ro_enable=1 ORDER BY ro_pos ASC LIMIT 1");
		if($db->Record) $oid=$db->Record["row_id"];
		else return false;
		return prepend_type($oid,'row');
};
$cmd['next_simple'][CMD_ROW]->result=CMD_ROW;
$cmd['next_simple'][CMD_ROW]->visual=Array('title'=>'Следующий (упрощ.)','group'=>GROUP_ROW_NAV);


$cmd['prev_simple'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $db;
		getrow($db,"SELECT * FROM row_owner WHERE row_table=".$rlink[$row]->table." AND ro_ex=".$rlink[$row]->tex." AND owner_id=".$rlink[$row]->owner." AND ro_pos<".$rlink[$row]->pos." AND ro_enable=1 ORDER BY ro_pos DESC LIMIT 1");
		if($db->Record) $oid=$db->Record["row_id"];
		else return false;
		return prepend_type($oid,'row');
};
$cmd['prev_simple'][CMD_ROW]->result=CMD_ROW;
$cmd['prev_simple'][CMD_ROW]->visual=Array('title'=>'Предыдущий (упрощ.)','group'=>GROUP_ROW_NAV);


$cmd['e5'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		return $r;
};
$cmd['e5'][CMD_ROW]->result=CMD_ROW;


$cmd['eid'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $db;
		getrow($db,"SELECT * FROM row_owner WHERE row_id=$row",1,"row_owner",__LINE__,__FILE__);
		seek_rlink($db->Record);
		$res=0;
		if(!empty($db->Record["ro_id"])) $res=$db->Record["ro_id"];
		return $res;		
};
$cmd['eid'][CMD_ROW]->result=CMD_STRING;
$cmd['eid'][CMD_ROW]->visual=Array('title'=>'Идентификатор экземпляра объекта','hidden'=>1);


$cmd['creation_date'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		return getrowval("SELECT creation_date FROM main_row WHERE row_id=$row",'creation_date');
};
$cmd['creation_date'][CMD_ROW]->result=CMD_STRING;
$cmd['creation_date'][CMD_ROW]->visual=Array('title'=>'Дата-время создания','result'=>STRING_DATETIME,'group'=>GROUP_ROW_PARAM);


$cmd['edit_date'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		return getrowval("SELECT modified_date FROM main_row WHERE row_id=$row",'modified_date');
};
$cmd['edit_date'][CMD_ROW]->result=CMD_STRING;
$cmd['edit_date'][CMD_ROW]->visual=Array('title'=>'Дата-время последнего редактирования','result'=>STRING_DATETIME,'group'=>GROUP_ROW_PARAM);


$cmd['rid'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		return $row;
};
$cmd['rid'][CMD_ROW]->result=CMD_STRING;
$cmd['rid'][CMD_ROW]->visual=Array('title'=>'Идентификатор объекта','hidden'=>1);


$cmd['edit?'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		$ex=get_ex($row,$table);	
		$res=check_row($row,$rlink[$row]->table,$ex,'edit',$rlink[$row]->user,$rlink[$row]->users);
		return $res;
};
$cmd['edit?'][CMD_ROW]->result=CMD_STRING;
$cmd['edit?'][CMD_ROW]->visual=Array('title'=>'Можно редактировать?','group'=>GROUP_ROW_ACC);


$cmd['del?'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		$ex=get_ex($row,$table);
		$res=check_row($row,$rlink[$row]->table,$ex,'del',$rlink[$row]->user,$rlink[$row]->users);
		return $res;
};
$cmd['del?'][CMD_ROW]->result=CMD_STRING;
$cmd['del?'][CMD_ROW]->visual=Array('title'=>'Можно удалять?','group'=>GROUP_ROW_ACC);


$cmd['del'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		
		$c=find_cmd('delrow',CMD_BASE,$row);
		$tmp=do_cmd($op,$ops,$c,$row);
		
		return false;
};
$cmd['del'][CMD_ROW]->result=CMD_NONE;
$cmd['del'][CMD_ROW]->visual=Array('title'=>'Удалить','group'=>GROUP_ROW_OPS);
$cmd['delete'][CMD_ROW]->alias=&$cmd['del'][CMD_ROW];


$cmd['view?'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		$ex=get_ex($row,$table);
		$res=check_row($row,$rlink[$row]->table,$ex,'view',$rlink[$row]->user,$rlink[$row]->users);
		return $res;
};
$cmd['view?'][CMD_ROW]->result=CMD_STRING;
$cmd['view?'][CMD_ROW]->alias=&$cmd['view'][CMD_ROW];
$cmd['view?'][CMD_ROW]->visual=Array('title'=>'Можно просматривать?','group'=>GROUP_ROW_ACC);


$cmd['neighbors'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $gsres;
		$owner=$rlink[$row]->owner;
		$ex=get_ex($row,$table);
		$ex1=get_tex($row,$ex,$table);
		if(!empty($ex)) $seek_ex=1; else $seek_ex=0;
		$seek_ena=1;
		$stable='';
		$ignore_table=0;
		if(!isset($gsres[$owner][$table][$seek_ena][$seek_ex][$ex][$ex1][$ignore_table][0])){
			get_sub($owner,$table,$seek_ena,$seek_ex,$ignore_table,0,0,$ex1,$ex,$table);
		}
		$gs->rows=$gsres[$owner][$table][$seek_ena][$seek_ex][$ex][$ex1][$ignore_table][0];
		$var[0]=$op->data;
		if(empty($var[0][9]) || ($var[0][9]!='2' && (empty($var[0][11]) || $var[0][11]!='a'))) if(!empty($gs->rows)){//and_i
			$tmp=Array();
			foreach($gs->rows AS $v=>$value){
				if($value->id!=$row) $tmp[$v]=$value;
			}
			$gs->rows=$tmp;
		}
		if(empty($var[0][9]) || ($var[0][9]!='3' && (empty($var[0][11]) || $var[0][11]!='s'))) if(!empty($gs->rows)) foreach($gs->rows AS $v=>$value){//sub
			unset($gs->rows[$v]->sub);
		}
		$gs->table=$table;
		$tex=get_tex($owner,$ex,$table);
		$gs->tex=$tex;
		$gs->owner=$owner;
		$gs->stable=$stable;	
		return $gs;
};
$cmd['neighbors'][CMD_ROW]->result=CMD_ARRAY;
$cmd['neighbors'][CMD_ROW]->result_long=CMD_ROW;
$cmd['neighbors2'][CMD_ROW]->alias=&$cmd['neighbors'][CMD_ROW];
$cmd['neighbors_and_i'][CMD_ROW]->alias=&$cmd['neighbors'][CMD_ROW];
$cmd['neighbors3'][CMD_ROW]->alias=&$cmd['neighbors'][CMD_ROW];
$cmd['neighbors_wo_sub'][CMD_ROW]->alias=&$cmd['neighbors'][CMD_ROW];
$cmd['neighbors'][CMD_ROW]->visual=Array('title'=>'Соседи','group'=>GROUP_ROW_NAV);
$cmd['neighbors_and_i'][CMD_ROW]->visual=Array('title'=>'Соседи (включая текущий объект)','group'=>GROUP_ROW_NAV);
$cmd['neighbors_sub'][CMD_ROW]->visual=Array('title'=>'Соседи (исключая дочерние объекты)','group'=>GROUP_ROW_NAV);


$cmd['wayback'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		if(!empty($rlink[$row])){
			$tmp=Array();
			$tm=new rlink();
			$tm=dc($rlink[$row]);//а тут это комментить нельзя, т.к. тут идёт sub_unset
			unset($tm->sub);
			$tmp[]=$tm;//? может этого и не надо
			$v=new rlink();
			$v=dc($rlink[$row]);
			while(!empty($v->owner) && $v->ownert==$v->table){
				if(empty($rlink[$v->owner])) seek_rlink($v->owner);
				$v=dc($rlink[$v->owner]);
				unset($v->sub);
				$tmp[]=$v;
			}
			return $tmp;
		}
		return false;
};
$cmd['wayback'][CMD_ROW]->result=CMD_ARRAY;
$cmd['wayback'][CMD_ROW]->result_long=CMD_ROW;
$cmd['wayback'][CMD_ROW]->visual=Array('title'=>'Массив родительских объектов (в рамках текущей таблицы)','group'=>GROUP_ROW_ARR);


$cmd['allway'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		if(!empty($rlink[$row])){
			$tmp=Array();
			$tm=new rlink();
			$tm=dc($rlink[$row]);//а тут это комментить нельзя, т.к. тут идёт sub_unset
			unset($tm->sub);
			$tmp[]=$tm;//? может этого и не надо
			$v=new rlink();
			$v=dc($rlink[$row]);
			while(!empty($v->owner)){
				if(empty($rlink[$v->owner])) seek_rlink($v->owner);
				$v=dc($rlink[$v->owner]);
				unset($v->sub);
				$tmp[]=$v;
			}
			return $tmp;
		}
		return false;
};
$cmd['allway'][CMD_ROW]->result=CMD_ARRAY;
$cmd['allway'][CMD_ROW]->result_long=CMD_ROW;
$cmd['allway'][CMD_ROW]->visual=Array('title'=>'Массив всех родительских объектов','group'=>GROUP_ROW_ARR);


$cmd['wayback2'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		if(!empty($rlink[$row])){
			$tmp=Array();
			$tmp[]=$row;//? может этого и не надо
			$v=new rlink();
			$v=$rlink[$row];//см. ниже в wayback3
			while(!empty($v->owner) && $v->ownert==$v->table){
				if(empty($rlink[$v->owner])) seek_rlink($v->owner);
				$v=$rlink[$v->owner];
				$tmp[]=$v->id;
			}
			return $tmp;
		}
		return false;
};
$cmd['wayback2'][CMD_ROW]->result=CMD_ARRAY;
$cmd['wayback2'][CMD_ROW]->result_long=CMD_ROW;


$cmd['wayback_url'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		if(!empty($rlink[$row])){
			$tmp=Array();
			$tmp[]=url_fromrow($row,$table);
			$v=new rlink();
			$v=$rlink[$row];	//dc приводила к сжиранию ВСЕЙ оперативной памяти (57мб из ничего
			$x=0;
			while(!empty($v->owner) && $v->ownert==$v->table){
				if(empty($rlink[$v->owner])) seek_rlink($v->owner);
				$v=$rlink[$v->owner];
				$tmp[]=url_fromrow($v->id,$table);
			}
			krsort($tmp);
			$res=implode('/',$tmp);//странная ошибка
			return $res;
		}
		return '';
};
$cmd['wayback_url'][CMD_ROW]->result=CMD_STRING;
$cmd['wayback3'][CMD_ROW]->alias=&$cmd['wayback_url'][CMD_ROW];
$cmd['wayback_url'][CMD_ROW]->visual=Array('title'=>'Url до текущего объекта, учитывая его родителей в рамках текущего объекта','group'=>GROUP_ROW_URL);


$cmd['allway_url'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		if(!empty($rlink[$row])){
			$tmp=Array();
			$tmp[]=url_fromrow($row,$table);
			$v=new rlink();
			$v=$rlink[$row];	//dc приводила к сжиранию ВСЕЙ оперативной памяти (57мб из ничего
			$x=0;
			while(!empty($v->owner)){
				if(empty($rlink[$v->owner])) seek_rlink($v->owner);
				$v=$rlink[$v->owner];
				$tmp[]=url_fromrow($v->id,$table);
			}
			krsort($tmp);
			$res=implode('/',$tmp);//странная ошибка
			return $res;
		}
		return '';
};
$cmd['allway_url'][CMD_ROW]->result=CMD_STRING;
$cmd['allway3'][CMD_ROW]->alias=&$cmd['allway_url'][CMD_ROW];
$cmd['allway_url'][CMD_ROW]->visual=Array('title'=>'Url до текущего объекта','group'=>GROUP_ROW_URL);


$cmd['if'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		if(isset($op->put[0]) && !empty($data)) return parse_var($op->put[0]);
		if(isset($op->put[1]) && empty($data)) return parse_var($op->put[1]);
		return $data;
};
$cmd['if'][CMD_ROW]->result=CMD_MIXED;
$cmd['if'][CMD_ROW]->visual=Array('title'=>'условие','work_on'=>CMD_LOGICAL,'put'=>Array(
		0=>Array('title'=>'Действие на выполнение условия','type'=>CMD_CMD,'req'=>1),
		1=>Array('title'=>'Иначе','type'=>CMD_CMD,'req'=>0)
),'group'=>GROUP_ROW_SPEC);


$cmd['ex'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		$v=new rlink();
		$v=dc($rlink[$row]);
		$ex=get_ex2($v->tex);
		return prepend_type($ex,'ex');
};
$cmd['ex'][CMD_ROW]->result=CMD_EX;
$cmd['ex'][CMD_ROW]->visual=Array('title'=>'Экземпляр объекта','group'=>GROUP_ROW_PARAM);


$cmd['geturl'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $db;
		if($op->data=='geturl') $c=url_col($table);
		if($op->data=='major') $c=seek_major($table);
		if(empty($c)) return false;
		$c_data=shift_first_op($ops,'col');
		$next_col_op=(!empty($ops) && !empty($ops[key($ops)]->data) && isset($GLOBALS['cmd'][$ops[key($ops)]->data][CMD_COL]));
		if(!$c_data && !$next_col_op || empty($ops) && !$c_data){
			global $rowval;
			if(!empty($rowval[$row][$c["col_sname"]])) $res=$rowval[$row][$c["col_sname"]];
			else {
				getrow($db,"SELECT * FROM row_value WHERE value_row=$row AND value_col=".$c["col_id"],1,"row_value",__LINE__,__FILE__);
				if(empty($db->Record)){ return false;}
				if(!empty($c["col_force_onshow"])) $db->Record["value_value"]=force_value($db->Record,$c);
				$rowval[$row][$c["col_sname"]]=$db->Record["value_value"];
				$res=$rowval[$row][$c["col_sname"]];
			}
			return $res;
		}
		if(count($ops)>0 && ($c_data || $next_col_op)){
			return prepend_type($c["col_id"],'cl',Array('row'=>$row,'sname'=>$c["col_sname"]));
		}
};
$cmd['geturl'][CMD_ROW]->result=CMD_MIXED;
$cmd['major'][CMD_ROW]->alias=&$cmd['geturl'][CMD_ROW];
$cmd['geturl'][CMD_ROW]->visual=Array('title'=>'URL поле объекта','result'=>CMD_COL,'group'=>GROUP_ROW_URL);
$cmd['major'][CMD_ROW]->visual=Array('title'=>'Главное поле объекта','result'=>CMD_COL,'group'=>GROUP_ROW_PARAM);


$cmd['sub'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		if(isset($rlink[$row]) && !isset($rlink[$row]->sub)){
		
			if(isset($GLOBALS["spec_step2"]) && !empty($GLOBALS["current"]) && $GLOBALS["current"]==$row){
				global $tree_vars2;
				$tree_vars2[$GLOBALS["spec_step2"]][$rlink[$row]->table]=$rlink[$row]->table.'.1.0.0.'.$rlink[$row]->tex.'.0.0';
			}
		
			$rlink[$row]->sub=get_sub($rlink[$row]->id,$rlink[$row]->table,1,0,0,0,0,$rlink[$row]->tex);
		}
		if(isset($rlink[$row]) && isset($rlink[$row]->sub)){
			return $rlink[$row]->sub;
		}
		return false;
};
$cmd['sub'][CMD_ROW]->result=CMD_ARRAY;
$cmd['sub'][CMD_ROW]->result_long=CMD_ROW;
$cmd['sub'][CMD_ROW]->visual=Array('title'=>'Массив дочерних элементов','group'=>GROUP_ROW_ARR);


$cmd['allsub'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		$res=Array();
		$first=true;
		while(!empty($search) || $first){
			if($first) $search=Array($row);
			$search=get_sub($search,$rlink[$row]->table,1,0,0,0,0,$rlink[$row]->tex,0,$rlink[$row]->table,0,0,0,Array(),0,0);
			foreach($search AS $s) $res[]=$s;
			$first=false;
		}
		return $res;
};
$cmd['allsub'][CMD_ROW]->result=CMD_ARRAY;
$cmd['allsub'][CMD_ROW]->result_long=CMD_ROW;
$cmd['allsub'][CMD_ROW]->visual=Array('title'=>'Массив дерева дочерних элементов','group'=>GROUP_ROW_ARR);


$cmd['up'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $db;
		//для единственного родителя
		if(empty($row)) return false;
		$oid=$rlink[$row]->owner;
		if(!isset($rlink[$row]->ownert)){//uncknown error
			if(!empty($oid)) $tid=getrowval("SELECT * FROM main_row WHERE row_id=$oid","row_table");
			else $tid=0;
		} else $tid=$rlink[$row]->ownert;
		//ниже участок под вопросом
		if($rlink[$row]->owner==0 && isset($rlink[$row]->rsub) && $rlink[$row]->rsub!=0){
				getrow($db,"SELECT * FROM main_row WHERE row_id=".$rlink[$row]->rsub,1,"main_row",__LINE__,__FILE__);
				$oid=$rlink[$row]->id;
				$tid=$rlink[$row]->table;
		}
		//конец участка под вопросом
		if($oid==0) return false;
		return prepend_type($oid,'row',Array('table'=>$tid));
};
$cmd['up'][CMD_ROW]->result=CMD_ROW;
$cmd['owner'][CMD_ROW]->alias=&$cmd['up'][CMD_ROW];
$cmd['up'][CMD_ROW]->visual=Array('title'=>'Родитель','group'=>GROUP_ROW_NAV);


//$cmd['owner'][CMD_ROW]->process=function(&$data,&$op,&$ops){
//		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
//		if(!empty($rlink[$row]) || empty($rlink[$row]->owner)) return false;
//		$v=new rlink();
//		$v=dc($rlink[$row]);
//		return prepend_type($v->owner,'row');
//};
//$cmd['owner'][CMD_ROW]->result=CMD_ROW;


$cmd['up2'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $db;
		getrow($db,"SELECT * FROM row_owner WHERE row_id=$row",1,"row_owner",__LINE__,__FILE__);
		seek_rlink($db->Record);
		if(empty($db->Record)) return false;
		$r->id=$db->Record["owner_id"];
		return prepend_type($r->id,'row');
};
$cmd['up2'][CMD_ROW]->result=CMD_ROW;


$cmd['ups'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $db;
		$r=Array();
		$a=getall($db,"SELECT * FROM row_owner WHERE row_id=$row",1,"row_owner",__LINE__,__FILE__);
		if(empty($a)) return false;
		for($i=0;$i<count($a);$i++){
			seek_rlink($a[$i]);
			seek_rlink($a[$i]["owner_id"]);
			$r[$i]=new rlink();
			$r[$i]=dc($rlink[$a[$i]["owner_id"]]);
		}
		return $r;
};
$cmd['ups'][CMD_ROW]->result=CMD_ARRAY;
$cmd['ups'][CMD_ROW]->result_long=CMD_ROW;
$cmd['owners'][CMD_ROW]->alias=&$cmd['ups'][CMD_ROW];
$cmd['ups'][CMD_ROW]->visual=Array('title'=>'Массив родителей','group'=>GROUP_ROW_ARR);


$cmd[''][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		global $rowcol,$rowval,$rlink,$cacherow,$rowprotect;
		global $db,$utpl;
		if(!isset($rowcol[$table]) && !empty($table)){
			$rowcol[$table]=Array();
			$tmp=getall($db,"SELECT * FROM main_col WHERE col_table=$table",1,"main_col",__LINE__,__FILE__);
			if(!empty($tmp)) foreach($tmp AS $tm) $rowcol[$table][$tm["col_sname"]]=$tm;
		}
		$var[0]=$op->data;
		if(isset($rowcol[$table][$var[0]])){;
			$ccol=$rowcol[$table][$var[0]];
			$utpl['row'][$row]=1;
			//ниже происходит кеширование строк. проблема в том что они кешируются невзирая на их экземпляр
			//по этому если экземпляров у этого модуля много - будет большое число затронутых строк при SQL запросе
			//но зато будет мало запросов к базе (что в конце-концов экономит время)
			//а если это кеширование отключить то кол-во запросов к базе возрастёт в 2 раза, но зато будет меньше кол-во затронутых строк
			$use_cache=0; // 0 - отключить кеш, 1 - обычный кеш, 2 - экспериментальный кеш
			//в целом после проверок оказалось что экспереминтальный кеш работает аналогично его отключению :)
			do_log('Cache rows type '.$use_cache);
			global $tree_vars;
			if(isset($GLOBALS["spec_step2"])) $tree_vars[$GLOBALS["spec_step2"]][$ccol["col_id"]]=$ccol;
			
			//обычный вариант
			if(!isset($cacherow[$table][$ccol["col_id"]]) && $table!=0 && $use_cache && $use_cache!=2){
				$cacherow[$table][$ccol["col_id"]]=1;
				//тут проблемотоз, потому что эта штука грузит дофига строк. надо хотябы ex ограничивать (а его то в value вообще нет!!)
				$vals=getall($db,"SELECT * FROM row_value WHERE value_col=".$ccol["col_id"],1,"row_value",__LINE__,__FILE__);				
				for($i=0;$i<count($vals);$i++){
					$val=$vals[$i];
					$crow=$val["value_row"];
					$rowprotect[$crow][$ccol["col_sname"]]=1;
					if(!empty($ccol["col_force_onshow"])) $val["value_value"]=force_value($val,$ccol);
					if($val["value_value"]!='0'){
						if($ccol["col_link2"]==0){
							$rowval[$crow][$ccol["col_sname"]]=$val["value_value"];
						}
						else {
							seek_rlink($val["value_value"]);
							if(isset($val["value_value"]) && isset($rlink[$val["value_value"]]) && $rlink[$val["value_value"]]->enable!=-1 && $rlink[$val["value_value"]]->enable==1){
								if(empty($rowval[$crow][$ccol["col_sname"]])) $rowval[$crow][$ccol["col_sname"]]=Array();
								$rowval[$crow][$ccol["col_sname"]][]->id=$val["value_value"];
							}
						}
					} else {
						$rowval[$crow][$ccol["col_sname"]]=0;//непонятная строчка
						//if($ccol['col_type']==0) $rowval[$crow][$ccol["col_sname"]]='0';
						//else $rowval[$crow][$ccol["col_sname"]]='';
					}
				}
			}
			
			$col=$rowcol[$table][$var[0]];			
			$row2='';
			global $vcache,$vcache2,$tree_vars;
			if(!isset($rowval[$row][$var[0]]) && /*empty*/!isset($rowprotect[$row][$var[0]])){
				if($col["col_link2"]==0 || $col["col_type"]==0){
					if(isset($vcache2[$col["col_id"]][$row])/* && 1==2*/){//это ещё что за...
						$db->Record=$vcache2[$col["col_id"]][$row];//current(reset($vcache[$col["col_id"]][$row]));
					} else if(isset($vcache[$col["col_id"]][$row])){
						if(!empty($vcache[$col["col_id"]][$row])) foreach($vcache[$col["col_id"]][$row] AS $v) {
							$db->Record=$v;
							break;
						}
					} else {
						//дебаг! эта строчка на гаранте вызывается 2059 раз 
						getrow($db,"SELECT * FROM row_value WHERE value_col=".$col["col_id"]." AND value_row=$row",1,"row_value",__LINE__,__FILE__);
						//if(!empty($col["col_force_onshow"])) $db->Record["value_value"]=force_value($db->Record,$col);
						$vcache2[$col["col_id"]][$row]=$db->Record;
						if(!empty($db->Record["value_row"])) $vcache[$col["col_id"]][$row][$db->Record["value_row"]]=$db->Record;
						if(isset($GLOBALS["spec_step2"])) $tree_vars[$GLOBALS["spec_step2"]][$col["col_id"]]=$col;//на свой страх и риск
					}
					if($col["col_type"]!=0 && !empty($db->Record["value_value"]) || $col["col_type"]==0 && isset($db->Record["value_value"])){
						if(!empty($col["col_force_onshow"])) $db->Record["value_value"]=force_value($db->Record,$col);
						$row2=$db->Record["value_value"];
					}
				} else {
					if(isset($vcache[$col["col_id"]][$row])){
						$tmp=$vcache[$col["col_id"]][$row];
					} else {
						$tmp=getall($db,"SELECT * FROM row_value WHERE value_col=".$col["col_id"]." AND value_row=$row",1,"row_value",__LINE__,__FILE__);
						if(isset($GLOBALS["spec_step2"])) $tree_vars[$GLOBALS["spec_step2"]][$col["col_id"]]=$col;//на свой страх и риск
					}
					foreach($tmp AS $lol=>$tm){
						$val=$tm;
						if(!empty($col["col_force_onshow"])) $val["value_value"]=force_value($val,$col);
						$vcache[$val["value_col"]][$val["value_row"]][$val["value_id"]]=$val;
						seek_rlink($val["value_value"]);
						if(isset($val["value_value"]) && isset($rlink[$val["value_value"]]) && $rlink[$val["value_value"]]->enable!=-1 && $rlink[$val["value_value"]]->enable==1){
							if(empty($row2)) $row2=Array();
							$row2[]->id=$val["value_value"];
						}
					}
				}
				$rowval[$row][$var[0]]=$row2;
			} else $row2=$rowval[$row][$var[0]];		
						
			
			global $no_cache;
			if($no_cache){unset($vcache); unset($vcache2);}
			//etf('parse_row4');
			//if($row2=='') return '';	// эта строчка под сомнением, фактически она была поставлена для устранения непонятной ошибки
			
			if(shift_first_op($ops,'col')){
				return prepend_type($col["col_id"],'cl',Array('sname'=>$col["col_sname"],'row'=>$row));
			}
			if($col["col_type"]==3 && shift_first_op($ops,'clone')){
				$res=clone_col_file($col,$row);
				return $res;
			}
			if($col["col_type"]==3 && shift_first_op($ops,'rename')){
				new_rand_name($row,$col["col_id"]);
				return false;
			}
			if($col["col_type"]==3 && (shift_first_op($ops,'filesize') || shift_first_op($ops,'size'))){
				$res=rowcol_val($row,$col);
				if(file_exists(DOCUMENT_ROOT.$res)) $res=filesize(DOCUMENT_ROOT.$res);
				else $res=0;
				return $res;
			}
			if($col["col_type"]==3 && (  shift_first_op($ops,'get')  ||  shift_first_op($ops,'getfile') )){
				global $rowval;
				$res=rowcol_val($row,$col);
				if($res=='') return '';
				if(file_exists(DOCUMENT_ROOT.$res)) $res=file_get_contents2(DOCUMENT_ROOT.$res);
				else $res='';
				return $res;
			}
			if($col["col_type"]==3 && ($top=shift_first_op($ops,'set') || $top=shift_first_op($ops,'set_with_handler'))){
				// (почти дубль в parse_col, как и всё, что связано с file)
				//внимание! этот метод не проверяет размер папки и файла
				if(!check_row($row,$rlink[$row]->table,get_ex($row,$rlink[$row]->table,$rlink[$row]->tex),'edit',$rlink[$row]->user,$rlink[$row]->users)) return false;
				check_single($top->put,$GLOBALS["lex_func_sep"]);
				$data=parse_var($top->put[0]);
				//$tmp=explodeA('(',substr($var[1],0,strlen($var[1])-1),'(',')',1,'{','}',1);
				if(isset($top->put[1])){
					$ext=parse_var($top->put[1]);
					if(!empty($ext)) $ext='.'.$ext;
				} else {
					$ext='';
				}
				$res=set_col_file($data,$row,$col,$ext);
				if($top->data=='set_with_handler') on_col_edit($row,$col['col_id'],$col['col_oninsert'],$rlink[$row]->module,get_ex($row,$rlink[$row]->table,$rlink[$row]->tex),$rlink[$row]->table);
				update_row_state($row);					
				return false;
				//return $res;
			}
			if($col["col_type"]!=3 && ($top=shift_first_op($ops,'set') || $top=shift_first_op($ops,'set_with_handler'))){
				if(!check_row($row,$rlink[$row]->table,get_ex($row,$rlink[$row]->table,$rlink[$row]->tex),'edit',$rlink[$row]->user,$rlink[$row]->users)) return false;
				check_single($top->put,$GLOBALS["lex_func_sep"]);
				$data=parse_var($top->put[0]);
				if(is_object($data) && isset($data->id)) $data=$data->id;
				$data=prepend_value($data);
				clear_rowval_cache($col["col_id"],$row);
				del_cache('row',$row);
				$db->query('DELETE FROM row_value WHERE value_row='.$row.' AND value_col='.$col["col_id"].' AND value_table>0',3,'row_value',__LINE__,__FILE__);			
				$db->query('INSERT INTO row_value (value_module,value_table,value_col,value_row,value_value)
					VALUES ('.$rlink[$row]->module.','.$rlink[$row]->table.','.$col["col_id"].','.$row.',\''.$data.'\')',3,'row_value',__LINE__,__FILE__);
				if($top->data=='set_with_handler') on_col_edit($row,$col['col_id'],$col['col_oninsert'],$rlink[$row]->module,get_ex($row,$rlink[$row]->table,$rlink[$row]->tex),$rlink[$row]->table);
				rebuild_row_index($row,$col["col_id"]);
				update_row_state($row);
				return false;
			}
			if(shift_first_op($ops,'show')){
				global $f_type;
				$oldft=$f_type;
				$f_type='onshow';
				$r=backup_globals();
				$GLOBALS["cur_table"]=$col["col_table"];
				$GLOBALS["cur_row"]=$row;
				unset($GLOBALS["cur_col"]);
				$GLOBALS["cur_col"]->id=$col["col_id"];
				$GLOBALS["cur_col"]->row=$row;
				//$GLOBALS["cur_col"]=$col["col_sname"];
				$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$GLOBALS["cur_row"];
				$res=shell_tpl($col["col_onshow"]);
				return_globals($r);
				$f_type=$oldft;
				return $res;
			}
			if(shift_first_op($ops,'form')){
				return show_form($col,$col["col_table"],$row);
			}
			//вложение ниже сделано для конструкций вида row.module.auto, чтобы можно было вызывать модуль не только текущей но и произвольного row
			/*if($col["col_type"]==4){
				if(!empty($GLOBALS["cur_row"]) && $GLOBALS["cur_row"]!=$row){
					global $left_url,$right_url;
					$lu_back=$left_url;
					$ru_back=$right_url;
					if(!empty($rlink[$row])){ 
						$left_url[]=url_fromrow($row,$table);
						$v=new rlink();
						$v=dc($rlink[$row]);
						while(!empty($v->owner)){
							$v=dc($rlink[$v->owner]);
							$left_url[]=url_fromrow($v->id,$table);
						}
					}
				}
			}*/
			if($row2!=''){
				//$do_mod=false;
				if($col["col_type"]==4){
					$row2=explode(':',$row2);
					if(!$col['module_url'] && !empty($row2[2])){
						//part
						return prepend_type($row2[2],'prt',Array('ex'=>$row2[1],'module'=>$row2[0]));
					} else {
						//module
						return prepend_type($row2[1],'ex',Array('module'=>$row2[0]));
					}
				}
				/*if($col["col_type"]==4 && shift_first_op($ops,'major')){
					$tid=0;
					if(strstr($row2,':')){
						$row2=explode(':',$row2);
						$mod=$row2[0];
						$ex=$row2[1];
						if(isset($row2[2])){
							$tid=getrowval("SELECT part_table, part_id, part_module FROM main_part WHERE part_id=".$row2[2]." AND part_module=$mod","part_table");
						}
					} else {
						$mod=$row2;
						$ex=0;
					}
					if(empty($tid)) $tid=getrowval("SELECT table_bold, table_module, table_id FROM main_table WHERE table_module=$mod AND table_bold=1","table_id");
					return prepend_type($tid,'tbl',Array('ex'=>$ex,'module'=>$mod));
				}
				if($col["col_type"]==4 && shift_first_op($ops,'auto')){
					//$do_mod=true;
					if(!empty($GLOBALS["cur_row"]) && $GLOBALS["cur_row"]!=$row){
						global $left_url,$right_url;
						$lu_back=$left_url;
						$ru_back=$right_url;
						if(!empty($rlink[$row])){ 
							$left_url[]=url_fromrow($row,$table);
							$v=new rlink();
							$v=dc($rlink[$row]);
							while(!empty($v->owner)){
								$v=dc($rlink[$v->owner]);
								$left_url[]=url_fromrow($v->id,$table);
							}
						}
					}
					if(substr_count($row2,':')>1){//>1 потому что двоеточий ДВЕ а не ТРИ
						$row3=explode(':',$row2);
						if($row3[0]!=$GLOBALS["cur_module"]){
							//$tmp=shell_module($row3[0].':'.$row3[1],$row3[2]);														
							$tmp=shell_module($row2);
							
							if(!empty($lu_back)){$left_url=$lu_back;$right_url=$ru_back;}
						} else {
							$tmp=shell_part($row3[2],$row3[1]);
						}					
					} else $tmp=shell_module($row2);
					if(!empty($lu_back)){$left_url=$lu_back;$right_url=$ru_back;}
					return $tmp;
				}
				if($col["col_type"]==4 && substr_count($row2,':')>1 && shift_first_op($ops,'shell')){
					$row3=explode(':',$row2);
					//$do_mod=true;
					//в общем суть такова. вначале было просто shell_part
					//но таким макаром не работают COW, если речь идёт например о блоках одного модуля, которые вызываются в другом
					//сделал shell_module (также как в if($col["col_type"]==4 && count($var)>1) ),
					//но тут перестало работать COW когда часть вызывается в рамках этого же модуля
					//после размышлений длинной почти в сутки плюнул и сделал иф - если модуль не равен текущему то пусть отрабатывается
					//а если равен то пусть вызывается как часть
	
					//но это не будет канать если будет сложная операция по парсингу URL, т.к проблемма то кроется именно там
					//интересно, что cow.module.title в шапке шаблона отрабатывает...
	
					//UPD в функции shell_module нужно было заменить функцию is_num на стандартную is_numeric
					
					if(!empty($GLOBALS["cur_row"]) && $GLOBALS["cur_row"]!=$row){
						global $left_url,$right_url;
						$lu_back=$left_url;
						$ru_back=$right_url;
						if(!empty($rlink[$row])){ 
							$left_url[]=url_fromrow($row,$table);
							$v=new rlink();
							$v=dc($rlink[$row]);
							while(!empty($v->owner)){
								$v=dc($rlink[$v->owner]);
								$left_url[]=url_fromrow($v->id,$table);
							}
						}
					}
	
					if($row3[0]!=$GLOBALS["cur_module"]){
						$tmp=shell_module($row3[0].':'.$row3[1],$row3[2]);
						if(!empty($lu_back)){$left_url=$lu_back;$right_url=$ru_back;}
					} else {
						$tmp=shell_part($row3[2],$row3[1]);
					}
					return $tmp;
				}
				if($col["col_type"]==4 && shift_first_op($ops,'module')){
					$row2=explode(':',$row2);
					//$do_mod=true;
					return prepend_type($row2[0],'md');
				}
				if($col["col_type"]==4 && shift_first_op($ops,'ex')){
					$row2=explode(':',$row2);
					return prepend_type($row2[1],'ex',Array('module'=>$row2[0]));
				}*/
				
								
				
				/*if($col["col_type"]==4 && shift_first_op($ops,'name')){
					//видимо, выдаёт имя части, назначенной в модуле
					$row2=explode(':',$row2);
					//$do_mod=true;
					if(!empty($row2[2])){
						getrow($db,"SELECT * FROM main_part WHERE part_id=$row2[2]",1,"main_part",__LINE__,__FILE__);
						if(!empty($db->Record)) $GLOBALS["part_cache4"][$db->Record["part_id"]]=$db->Record;
						$res=$db->Record["part_name"];
						return $res;
					} else return false;
				}*/
				/*if($col["col_type"]==4 && shift_first_op($ops,'modex')){
					$row2=explode(':',$row2);
					$do_mod=true;
					$res=$row2[0].':'.$row2[1];
					return $res;
				}*/
				/*if($col["col_type"]==4 && !$do_mod && shift_first_op($ops,'mod')){
					$row2=explode(':',$row2);
					$do_mod=true;
					return prepend_type($row2[0],'md',Array('ex'=>$row2[1]));
				}*/
				
				//Эти два нижних надо объеденить в один, и вообще надо что-то думать, особенно по части url_find-ера
				/*if($col["col_type"]==4 && !empty($ops) && !$do_mod && !empty($ops[key($ops)]->data) && isset($GLOBALS['cmd'][$ops[key($ops)]->data][CMD_MODULE])){//из-за закомментированного в частности не выполнялось, например, cow.module.title / перенесено в обработку экземпляра
					$top=array_shiftA($ops);
					$tmp=shell_module($row2,$top->data);
					if(!empty($lu_back)){$left_url=$lu_back;$right_url=$ru_back;}
					$do_mod=true;
					return $tmp;
				}*/
				//из-за этого очень сильно тормозит, но без этого не пашет cow.module.title конструкции
				/*if($col["col_type"]==4 && !empty($ops) && !$do_mod && !empty($ops[key($ops)]->data) && !isset($GLOBALS['cmd'][$ops[key($ops)]->data][CMD_MODULE])){
					if(strstr($row2,':')){
						$mod=explode(':',$row2);
						$mod=$mod[0];
					} else $mod=$row2;
					if(getrowval("SELECT part_sname FROM main_part WHERE part_module=$mod AND part_sname='".$ops[key($ops)]->data."'","part_sname")){
						$top=array_shiftA($ops);
						$tmp=shell_module($row2,$top->data,0,$top);
						if(!empty($lu_back)){$left_url=$lu_back;$right_url=$ru_back;}
						$do_mod=true;
						return $tmp;
					}
				}*/
				
				
				/*if($col["col_type"]==4 && !empty($ops) && !$do_mod){
					if(strstr($row2,':')){
						$row2=explode(':',$row2);
						$row2=$row2[1];
					}
					return prepend_type($row2,'ex');
				}*/
				if(($col["col_type"]==0 || $col["col_type"]==3)){
					return $row2;
				}
				if($col["col_type"]==5){
					if($col["col_link2"]==0 || (!is_array($row2) && (!is_object($row2) || !isset($row2->rows)))){
						return prepend_type($row2,'usr');
					} else {
						//обработчик множественного выбора пользователей
					}
				}
				if($col["col_type"]==1){
					if(isset($ind) && $indtype==2 && !empty($row2[$ind]->id)) $row2=$row2[$ind]->id;
					if(isset($ind) && $indtype==1) $row2=$ind; // WTF?										
					if($col["col_link2"]==0 || (!is_array($row2) && (!is_object($row2) || !isset($row2->rows)))){
						return prepend_type($row2,'row',Array('table'=>$table));
					} else {
						$res=Array();
						foreach($row2 AS $r) $res[]=get_rlink($r);
						return $res;
					}
				}
				return $row2;
			}
		}

		//if(!empty($ops) && isset($ops[key($ops)]->data) && $ops[key($ops)]->data=='sut'){
		if($var[0]=='sut'){
			$op=array_shiftA($ops);//get_first_op($ops,'sut');
			$var[0]=$op->data;
		}

		$ctable=check_table2($table,$var[0]);
		//сюда почему-то проходят некоторые параметры, которые должны были вывестись ранее, например cur.tz
		if(!empty($ctable)){
			$ex=0;
			if(empty($ex)) $ex=get_ex2($rlink[$row]->tex);
			return prepend_type($ctable,'tbl',Array('ex'=>$ex, 'own'=>$row, 'sname'=>$var[0]));
		} else if(!empty($table)){
			global $alone_table_cache;
			if(!isset($alone_table_cache[$table])) $alone_table_cache[$table]=getrowval("SELECT table_id, table_sname FROM main_table WHERE table_id='$table'","table_sname");
			$sname=$alone_table_cache[$table];
			if($var[0]==$sname){
				if(empty($ex)) $ex=get_ex2($rlink[$row]->tex);
				return prepend_type($table,'tbl',Array('ex'=>$ex,'own'=>$row,'sname'=>$var[0]));
			}
		}
		
		return false;
};
$cmd[''][CMD_ROW]->result=CMD_MIXED;
$cmd[''][CMD_ROW]->visual=Array('title'=>'','special'=>SP_UNKNOWN,'object_type'=>OB_TABLE,'pos'=>1);


$cmd['depend'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		$owner=parse_var($op->put);
		if(is_object($owner)) $owner=$owner->id;
		clone_row($owner,$row,1);
};
$cmd['depend'][CMD_ROW]->result=CMD_NONE;
$cmd['depend'][CMD_ROW]->visual=Array('title'=>'Клонировать','put'=>Array(
		0=>Array('title'=>'новый родитель','type'=>CMD_ROW,'req'=>1)
),'group'=>GROUP_ROW_OPS);


$cmd['selected?'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		return !empty($GLOBALS["selected_rows"][$row]);
};
$cmd['selected?'][CMD_ROW]->result=CMD_STRING;
$cmd['selected?'][CMD_ROW]->visual=Array('title'=>'Объект присутствует в URL?','group'=>GROUP_ROW_PARAM);


$cmd['with'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		$res=backup_globals();
		$GLOBALS['current']=$row;
		if(!isset($GLOBALS["spec_step"])) $GLOBALS["spec_step"]=0;
		if(!isset($GLOBALS["spec_step2"])) $GLOBALS["spec_step2"]=0;
		$GLOBALS["spec_step"]++;
		$GLOBALS["spec_step2"]++;			
		$r=parse_var($op->put);
		return_globals($res);
		return $r;
};
$cmd['with'][CMD_ROW]->result=CMD_MIXED;
$cmd['with'][CMD_ROW]->visual=Array('title'=>'Исполнить оператор','put'=>Array(
		0=>Array('title'=>'оператор','type'=>CMD_CMD,'req'=>1)
),'group'=>GROUP_ROW_SPEC);


//данная команда введена специально для поддержки двух типов ввода в редакторе модулей (кнопочный и ручной)
$cmd['col'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		$op->data=parse_var($op->put);
		unset($op->put);
		//array_unshiftA($ops,$op);
		//$op->next_operation='.';
		//$x=&$cmd[''][CMD_ROW]->process;
		$x=get_cmd_link('',CMD_ROW);
		return $x($data,$op,$ops);
};
$cmd['col'][CMD_ROW]->result=CMD_MIXED;
$cmd['col'][CMD_ROW]->visual=Array('title'=>'Неизвестное поле','result'=>CMD_MIXED,'spec_result'=>CMD_STRING,'put'=>Array(
		0=>Array('title'=>'Имя поля (на англ.)','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_ROW_PARAM);


$cmd['cols'][CMD_ROW]->process=function(&$data,&$op,&$ops){
		global $rlink; $r=get_rlink($data); if(!$r) return false; $table=$r->table; $row=$r->id;
		//$t=seek_table($data); if(empty($t['table'])) return false; $owner=$t['own']; $ex=$t['ex']; $tex=$t['tex']; $table=$t['id']; $t=$t['table']; $stable=$t['table_sname'];
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
			$res[$cr]->row=$row;
			$cr++;
		}
		return $res;
};
$cmd['cols'][CMD_ROW]->result=CMD_ARRAY;
$cmd['cols'][CMD_ROW]->result_long=CMD_COL;
$cmd['cols'][CMD_ROW]->visual=Array('title'=>'Массив полей','group'=>GROUP_ROW_PARAM);



?>