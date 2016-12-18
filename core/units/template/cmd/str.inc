<?php

define('GROUP_HTML',2001);
$cmd_group[GROUP_HTML]=Array('title'=>'Работа с HTML','pos'=>1);
define('GROUP_DATE',2002);
$cmd_group[GROUP_DATE]=Array('title'=>'Работа с датой/временем','pos'=>2);
define('GROUP_NUM',2003);
$cmd_group[GROUP_NUM]=Array('title'=>'Работа с числами','pos'=>3);
define('GROUP_TEXT',2004);
$cmd_group[GROUP_TEXT]=Array('title'=>'Работа с текстом','pos'=>4);
define('GROUP_SEARCH',2005);
$cmd_group[GROUP_SEARCH]=Array('title'=>'Поиск','pos'=>5);
define('GROUP_STR_SPEC',2006);
$cmd_group[GROUP_STR_SPEC]=Array('title'=>'Специфические','pos'=>7);
define('GROUP_STR_URL',2007);
$cmd_group[GROUP_STR_URL]=Array('title'=>'Работа с URL','pos'=>6);
define('GROUP_STR_FILE',2008);
$cmd_group[GROUP_STR_FILE]=Array('title'=>'Работа с файлами','pos'=>6);
define('GROUP_STR_TPL',2009);
$cmd_group[GROUP_STR_TPL]=Array('title'=>'Работа с шаблонами','pos'=>6);
define('GROUP_MATH',2010);
$cmd_group[GROUP_MATH]=Array('title'=>'Математические операции','pos'=>3);

$cmd['_function'][CMD_STRING]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_func'][CMD_STRING]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_component'][CMD_STRING]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_show'][CMD_STRING]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_form'][CMD_STRING]->alias=&$cmd['_function'][CMD_ROW];
$cmd['_part'][CMD_STRING]->alias=&$cmd['_function'][CMD_ROW];

/*$cmd['_function'][CMD_STRING]->visual=Array('title'=>'функции','put'=>Array(0=>Array('title'=>'функция','type'=>STRING_FUNCTION_NAME,'req'=>1)), 'special'=>SP_POINT,'extend_put'=>1);
$cmd['_show'][CMD_STRING]->visual=Array('title'=>'отображения','put'=>Array(0=>Array('title'=>'отображение','type'=>STRING_SHOW_NAME,'req'=>1)), 'special'=>SP_POINT,'extend_put'=>1);
$cmd['_component'][CMD_STRING]->visual=Array('title'=>'компоненты','put'=>Array(0=>Array('title'=>'компонент','type'=>STRING_COMPONENT_NAME,'req'=>1)), 'special'=>SP_POINT,'extend_put'=>1);
$cmd['_form'][CMD_STRING]->visual=Array('title'=>'формы','put'=>Array(0=>Array('title'=>'форма','type'=>STRING_FORM_NAME,'req'=>1)), 'special'=>SP_POINT,'extend_put'=>1);*/

$cmd['_function'][CMD_STRING]->visual=Array('title'=>'функции', 'special'=>SP_COMPONENT,'extend_put'=>1,'group'=>GROUP_STR_SPEC);
$cmd['_show'][CMD_STRING]->visual=Array('title'=>'отображения', 'special'=>SP_COMPONENT,'extend_put'=>1,'group'=>GROUP_STR_SPEC);
$cmd['_component'][CMD_STRING]->visual=Array('title'=>'компоненты', 'special'=>SP_COMPONENT,'extend_put'=>1,'group'=>GROUP_STR_SPEC);
$cmd['_form'][CMD_STRING]->visual=Array('title'=>'формы', 'special'=>SP_COMPONENT,'extend_put'=>1,'group'=>GROUP_STR_SPEC);

$cmd['_part'][CMD_STRING]->visual=Array('title'=>'части', 'special'=>SP_PART,'extend_put'=>1,'group'=>GROUP_STR_SPEC);


$cmd['shell'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$type=1;
		if(empty($str)) return false;
		$old_xbreak=!empty($GLOBALS["xbreak"]);
		if(strlen5($str)>30 || substr_count(':',$str)!=2) $type=0;
		if($type==1 && !is_num(str_replace(':','',$str))) $type=0;
		$cache_conf=get_cache_conf();

		// prepend params if exists		
		if(!empty($op->put)){
			global $p_par;
			check_single($op->put,$GLOBALS["lex_func_sep"]);
			$par=prepend_param($op->put,0,0,false);
		}		
		
		// shell
		if($type==0){
			$res=shell_tpl($str);
		} else {
			$str=explode(':',$str);
			$res=shell_part($str[2],$str[1]);
		}
		
		// clear params if declare
		if(!empty($op->put)){
			$p_par=$par;
		}				
		
		set_cache_conf($cache_conf);
		
		if(!empty($GLOBALS["xbreak"]) && !empty($GLOBALS["clear"]) && !$old_xbreak){
			//затычка для нормальной отработки return через shell
			$GLOBALS["xbreak"]="";
			$GLOBALS["clear"]="";
		}
		if(!empty($GLOBALS["xbreak"]) && !$old_xbreak){
			//затычка для нормальной отработки return через shell
			$GLOBALS["xbreak"]="";
		}

		
		return $res;
};
$cmd['shell'][CMD_STRING]->result=CMD_STRING;
$cmd['shell'][CMD_STRING]->visual=Array('title'=>'исполнить как шаблон','work_on'=>CMD_TPL,'group'=>GROUP_STR_TPL);


$cmd['if'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		if(isset($op->put[0]) && $str) return parse_var($op->put[0]);
		if(isset($op->put[1]) && !$str) return parse_var($op->put[1]);
		return $str;
};
$cmd['if'][CMD_STRING]->result=CMD_MIXED;
$cmd['if'][CMD_STRING]->visual=Array('title'=>'условие','work_on'=>CMD_LOGICAL,'put'=>Array(
		0=>Array('title'=>'Действие на выполнение условия','type'=>CMD_CMD,'req'=>1),
		1=>Array('title'=>'Иначе','type'=>CMD_CMD,'req'=>0)
),'group'=>GROUP_STR_SPEC);


$cmd['clear'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return clean_html($str);
};
$cmd['clear'][CMD_STRING]->result=CMD_STRING;
$cmd['clear'][CMD_STRING]->visual=Array('title'=>'очистить от HTML тегов','group'=>GROUP_HTML);


$cmd['is_numeric'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return is_numeric($str);
};
$cmd['is_numeric'][CMD_STRING]->result=CMD_STRING;
$cmd['is_numeric'][CMD_STRING]->visual=Array('title'=>'это число?','group'=>GROUP_NUM);


$cmd['is_nan'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return is_nan($str);
};
$cmd['is_nan'][CMD_STRING]->result=CMD_STRING;
$cmd['is_nan'][CMD_STRING]->visual=Array('title'=>'это не число?','group'=>GROUP_NUM);


$cmd['code'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$pwl=parse_var($op->put);
		return simple_string(e_code($str,$pwl));
};
$cmd['code'][CMD_STRING]->result=CMD_STRING;
$cmd['code'][CMD_STRING]->visual=Array('title'=>'шифровать','put'=>Array(
		0=>Array('title'=>'пароль','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_STR_SPEC);


$cmd['decode'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$pwl=parse_var($op->put);
		return e_decode(desimple_string($str),$pwl);
};
$cmd['decode'][CMD_STRING]->result=CMD_STRING;
$cmd['decode'][CMD_STRING]->visual=Array('title'=>'расшифровать','put'=>Array(
		0=>Array('title'=>'пароль','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_STR_SPEC);


$cmd['parse'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$cache_conf=get_cache_conf();
		if($str===true) $res=true;
		else $res=parse_var($str);
		set_cache_conf($cache_conf);
		return $res;
};
$cmd['parse'][CMD_STRING]->result=CMD_STRING;
$cmd['parse'][CMD_STRING]->visual=Array('title'=>'запустить как оператор','work_on'=>CMD_CMD,'group'=>GROUP_STR_TPL);


$cmd['length'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return mb_strlen($str,'UTF-8');
};
$cmd['length'][CMD_STRING]->result=CMD_STRING;
$cmd['length'][CMD_STRING]->visual=Array('title'=>'длинна','group'=>GROUP_TEXT,'result'=>STRING_NUM);


$cmd['basename'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return basename($str);
};
$cmd['basename'][CMD_STRING]->result=CMD_STRING;
$cmd['basename'][CMD_STRING]->visual=Array('title'=>'имя файла (из пути)','work_on'=>STRING_FILENAME,'group'=>GROUP_STR_FILE);


$cmd['dirname'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return dirname($str);
};
$cmd['dirname'][CMD_STRING]->result=CMD_STRING;
$cmd['dirname'][CMD_STRING]->visual=Array('title'=>'имя папки (из пути)','work_on'=>STRING_FILENAME,'group'=>GROUP_STR_FILE);


$cmd['smartsize'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return smart_size($str);
};
$cmd['smartsize'][CMD_STRING]->result=CMD_STRING;
$cmd['smartsize'][CMD_STRING]->visual=Array('title'=>'форматировать размер файла','group'=>GROUP_STR_FILE);


$cmd['ndate'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return get_normal_date($str);
};
$cmd['ndate'][CMD_STRING]->result=CMD_STRING;
$cmd['ndate'][CMD_STRING]->visual=Array('title'=>'дата в формате Ч М (словом) Г','work_on'=>STRING_DATE,'group'=>GROUP_DATE);


$cmd['md5'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return md5($str);
};
$cmd['md5'][CMD_STRING]->result=CMD_STRING;
$cmd['md5'][CMD_STRING]->visual=Array('title'=>'Хеш md5','group'=>GROUP_STR_SPEC);


$cmd['numeric'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return parseInt($str);
};
$cmd['numeric'][CMD_STRING]->result=CMD_STRING;
$cmd['numeric'][CMD_STRING]->visual=Array('title'=>'оставить одни числа','group'=>GROUP_NUM,'result'=>STRING_NUM);


$cmd['month'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if(!empty($op->put)){
			$v[1]=parse_var($op->put);
			return month_plus($str,$v[1]);
		} else {
			$res=explode('-',$str);
			if(isset($res[1])) return $res[1]; else return 0;
		}
};
$cmd['month'][CMD_STRING]->result=CMD_STRING;
$cmd['month'][CMD_STRING]->visual=Array('title'=>'меясц (из даты)','work_on'=>STRING_DATE,'put'=>Array(
		0=>Array('title'=>'добавить это количество месяцев и вернуть получившиюся дату','type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_DATE);


$cmd['month2'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return get_month($str);
};
$cmd['month2'][CMD_STRING]->result=CMD_STRING;
$cmd['month2'][CMD_STRING]->visual=Array('title'=>'количество месяцев от начала отсчёта времени до указанной даты','work_on'=>STRING_DATE,'group'=>GROUP_DATE);


$cmd['monthR'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$res=explode('-',$str);
		return get_spec_month2($res[1]);
};
$cmd['monthR'][CMD_STRING]->result=CMD_STRING;
$cmd['monthR'][CMD_STRING]->visual=Array('title'=>'Месяц прописью (из даты)','work_on'=>STRING_DATE,'group'=>GROUP_DATE);


$cmd['monthR2'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$res=explode('-',$str);
		return get_spec_month($res[1]);
};
$cmd['monthR2'][CMD_STRING]->result=CMD_STRING;
$cmd['monthR2'][CMD_STRING]->visual=Array('title'=>'Месяц прописью (из даты, род.падеж)','work_on'=>STRING_DATE,'group'=>GROUP_DATE);


$cmd['monthB'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return get_spec_month2($str);
};
$cmd['monthB'][CMD_STRING]->result=CMD_STRING;
$cmd['monthB'][CMD_STRING]->visual=Array('title'=>'Месяц прописью (из числа)','work_on'=>STRING_NUM,'group'=>GROUP_DATE);


$cmd['monthB2'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return get_spec_month($str);
};
$cmd['monthB2'][CMD_STRING]->result=CMD_STRING;
$cmd['monthB2'][CMD_STRING]->visual=Array('title'=>'Меясц прописью (из числа, род.падеж)','work_on'=>STRING_NUM,'group'=>GROUP_DATE);


$cmd['firstcaps'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return mb_substr(mb_strtoupper($str, "utf-8"),0,1,'utf-8').mb_substr($str,1,mb_strlen($str)-1,'utf-8');
};
$cmd['firstcaps'][CMD_STRING]->result=CMD_STRING;
$cmd['firstcaps'][CMD_STRING]->visual=Array('title'=>'поднять первую букву','group'=>GROUP_TEXT);


$cmd['trim'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if(empty($op->put)) return trim($str);
		else {
			$v=parse_var($op->put);
			$res=$str;
			$res2='';
			if(empty($res)) return $res;
			while($res[0]==$v && $v!='' && $res!='' && $res2!=$res){
				$res2=$res;
				$res=substr5($res,1,strlen5($res)-1);
			}
			$res2='';
			while($res[strlen5($res)-1]==$v && $v!='' && $res!='' && $res2!=$res){
				$res2=$res;
				$res=substr5($res,0,strlen5($res)-1,'utf-8');
			}
			return $res;
		}
};
$cmd['trim'][CMD_STRING]->result=CMD_STRING;
$cmd['trim2'][CMD_STRING]->alias=&$cmd['trim'][CMD_STRING];
$cmd['trim'][CMD_STRING]->visual=Array('title'=>'Убрать пробелы (или другие символы) с краёв строки','put'=>Array(
		0=>Array('title'=>'Символ для очистки','type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_TEXT);


$cmd['highlight'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if(is_object($str) && isset($str->id)){
			global $rlink,$db,$highlight_col_cache;
			$row=$str->id;
			seek_rlink($row);
			$table=$rlink[$row]->table;
			if(!isset($highlight_col_cache[$table])) $highlight_col_cache[$table]=getall3($db,"SELECT col_id FROM main_col WHERE col_table=$table AND col_index=1","col_id");
			$cols=$highlight_col_cache[$table];
			if(empty($cols)) return '';
			$cols=implode(',',$cols);
			$vals=getall6($db,"SELECT value_col, value_value FROM row_value WHERE value_row=$row AND value_table=$table AND value_col IN ($cols)","value_col","value_value");
			$str=implode(' ',$vals);
		}
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$query=parse_var($op->put[0]);
		if(!empty($op->put[1])) $class=parse_var($op->put[1]); else $class='';
		if(!empty($op->put[2])) $style=parse_var($op->put[2]); else $style='';
		if(!empty($op->put[3])) $step_word=parse_var($op->put[3]); else $step_word=3;
		if(!empty($op->put[4])) $max_word=parse_var($op->put[4]); else $max_word=40;
		if($op->data=='hightlight2' || $op->data=='hightlight_only'){
			$step_word=0;
			$max_word=0;
		}
		return add_span_to_text($query,$str,$class,$style,$step_word,$max_word);
};
$cmd['highlight'][CMD_STRING]->result=CMD_STRING;
$cmd['highlight2'][CMD_STRING]->alias=&$cmd['highlight'][CMD_STRING];
$cmd['highlight_only'][CMD_STRING]->alias=&$cmd['highlight'][CMD_STRING];

$cmd['highlight_only'][CMD_STRING]->visual=Array('title'=>'подсветка искомых фраз', 'put'=>Array(
		0=>Array('title'=>'Запрос','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'CSS класс выделенных слов','type'=>CMD_STRING,'req'=>0),
		2=>Array('title'=>'CSS стили выделенных элементов','type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_SEARCH);
$cmd['highlight'][CMD_STRING]->visual=Array('title'=>'выдать искомые фразы', 'put'=>Array(
		0=>Array('title'=>'Запрос','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'CSS класс выделенных слов','type'=>CMD_STRING,'req'=>0),
		2=>Array('title'=>'CSS стили выделенных элементов','type'=>CMD_STRING,'req'=>0),
		3=>Array('title'=>'Количество слов справа и слева от искомого','type'=>CMD_STRING,'req'=>0),
		4=>Array('title'=>'Максимальное количество выданных слов','type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_SEARCH);


/*
$cmd['highlight2'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$query=parse_var($op->put[0]);
		if(!empty($op->put[1])) $class=parse_var($op->put[1]); else $class='';
		if(!empty($op->put[2])) $style=parse_var($op->put[2]); else $style='';
		return add_span_to_text($query,$str,$class,$style,0);
};
$cmd['highlight2'][CMD_STRING]->result=CMD_STRING;
$cmd['highlight_only'][CMD_STRING]->alias=&$cmd['highlight2'][CMD_STRING];
*/


$cmd['lower'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return mb_strtolower($str, "utf-8");
};
$cmd['lower'][CMD_STRING]->result=CMD_STRING;
$cmd['lower'][CMD_STRING]->visual=Array('title'=>'к нижнему регистру','group'=>GROUP_TEXT);


$cmd['upper'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return mb_strtoupper($str, "utf-8");
};
$cmd['upper'][CMD_STRING]->result=CMD_STRING;
$cmd['upper'][CMD_STRING]->visual=Array('title'=>'к верхнему регистру','group'=>GROUP_TEXT);


$cmd['urlencode'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return urlencode($str);
};
$cmd['urlencode'][CMD_STRING]->result=CMD_STRING;
$cmd['urlencode'][CMD_STRING]->visual=Array('title'=>'закодировать для URL','group'=>GROUP_STR_URL);


$cmd['urldecode'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return urldecode($str);
};
$cmd['urldecode'][CMD_STRING]->result=CMD_STRING;
$cmd['urldecode'][CMD_STRING]->visual=Array('title'=>'раскодировать из URL','group'=>GROUP_STR_URL);


$cmd['replace_tag'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$t1=parse_var($op->put[0]);
		$t2=parse_var($op->put[1]);
		$content=parse_var($op->put[2]);
		if(!empty($op->put[3])) $del_tags=parse_var($op->put[3]); else $del_tags=false;
		if(!empty($op->put[4])) $except=parse_var($op->put[4]); else $except=false;
		$arr=get_all_tag($str,$t1,$t2);
		$res=$str;
		if(!empty($arr)){
			foreach($arr AS $t)if(empty($except) || !strstr($t,$except)){
				$tmp=str_replace('%data%',$t,$content);
				if(!$del_tags) $res=str_replace($t,$tmp,$res);
				else $res=str_replace($t1.$t.$t2,$tmp,$res);
			}
		}
		return $res;
};
$cmd['replace_tag'][CMD_STRING]->result=CMD_STRING;
$cmd['replace_tag'][CMD_STRING]->visual=Array('title'=>'заменить содержимое тегов','put'=>Array(
		0=>Array('title'=>'открывающий тег','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'закрывающий тег','type'=>CMD_STRING,'req'=>1),
		2=>Array('title'=>'новое содержимое','type'=>CMD_STRING,'req'=>1),
		3=>Array('title'=>'заменять вместе с тегами','type'=>CMD_LOGICAL,'req'=>0),
		4=>Array('title'=>'триггер (если он найден между тегами, то замена не происходит','type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_HTML);


$cmd['gettag_all'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$t1=parse_var($op->put[0]);
		$t2=parse_var($op->put[1]);
		return get_all_tag($str,$t1,$t2);
};
$cmd['gettag_all'][CMD_STRING]->result=CMD_ARRAY;
$cmd['gettag_all'][CMD_STRING]->result_long=CMD_STRING;
$cmd['gettag3'][CMD_STRING]->alias=&$cmd['gettag_all'][CMD_STRING];
$cmd['gettag_all'][CMD_STRING]->visual=Array('title'=>'выдать массив строк, содержащийся между тегами','put'=>Array(
		0=>Array('title'=>'открывающий тег','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'закрывающий тег','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_HTML);


$cmd['gettag_next'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$t1=parse_var($op->put[0]);
		$t2=parse_var($op->put[1]);
		return get_tag($str,$t1,$t2,true);
};
$cmd['gettag_next'][CMD_STRING]->result=CMD_STRING;
$cmd['gettag2'][CMD_STRING]->alias=&$cmd['gettag_next'][CMD_STRING];
$cmd['gettag_next'][CMD_STRING]->visual=Array('title'=>'выдать следующую строку, содержащуюся между тегами','put'=>Array(
		0=>Array('title'=>'открывающий тег','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'закрывающий тег','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_HTML);


$cmd['gettag'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$t1=parse_var($op->put[0]);
		$t2=parse_var($op->put[1]);
		return get_tag($str,$t1,$t2);
};
$cmd['gettag'][CMD_STRING]->result=CMD_STRING;
$cmd['gettag'][CMD_STRING]->visual=Array('title'=>'выдать первую строку, содержащуюся между тегами','put'=>Array(
		0=>Array('title'=>'открывающий тег','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'закрывающий тег','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_HTML);


$cmd['deltag'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$t1=parse_var($op->put[0]);
		$t2=parse_var($op->put[1]);
		return del_tag($str,$t1,$t2);
};
$cmd['deltag'][CMD_STRING]->result=CMD_STRING;
$cmd['deltag'][CMD_STRING]->visual=Array('title'=>'удалить строки, содержащиеся между тегами (вместе с тегами)','put'=>Array(
		0=>Array('title'=>'открывающий тег','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'закрывающий тег','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_HTML);


$cmd['day'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if(!empty($op->put)){
			$v[1]=parse_var($op->put);
			return day_plus($str,$v[1]);
		} else {
			$res=explode('-',$str);
			if(isset($res[2])) return $res[2]; else return 0;
		}
};
$cmd['day'][CMD_STRING]->result=CMD_STRING;
$cmd['day'][CMD_STRING]->visual=Array('title'=>'Выдать день из даты','work_on'=>STRING_DATE,'put'=>Array(
		0=>Array('title'=>'Количество дней, на которые нужно увеличить дату (возвращает дату)','type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_DATE);


$cmd['day2'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return get_day($str);
};
$cmd['day2'][CMD_STRING]->result=CMD_STRING;
$cmd['day2'][CMD_STRING]->visual=Array('title'=>'количество дней от начала отсчёта времени до указанной даты','work_on'=>STRING_DATE,'group'=>GROUP_DATE);


$cmd['day3'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return day_left($str);
};
$cmd['day3'][CMD_STRING]->result=CMD_STRING;
$cmd['day3'][CMD_STRING]->visual=Array('title'=>'количество дней от даты на сервере до указанной даты','work_on'=>STRING_DATE,'group'=>GROUP_DATE);


$cmd['day4'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return day_left2($str);
};
$cmd['day4'][CMD_STRING]->result=CMD_STRING;
$cmd['day4'][CMD_STRING]->visual=Array('title'=>'количество дней от указанной даты до даты на сервере','work_on'=>STRING_DATE,'group'=>GROUP_DATE);


$cmd['hour_passed'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$date=explode(' ',$str);
		if(empty($date[1])) $date[1]='00:00:00';
		$day=get_day($date[0]);
		$date=explode(':',$date[1]);
		$h=$date[0];
		return ($day-1)*24+$h;
};
$cmd['hour_passed'][CMD_STRING]->result=CMD_STRING;
$cmd['hour_passed'][CMD_STRING]->visual=Array('title'=>'кол-во часов от начала отсчёта времени','work_on'=>STRING_DATETIME,'group'=>GROUP_DATE);
$cmd['hour2'][CMD_STRING]->alias=&$cmd['hour_passed'][CMD_STRING];


$cmd['minute_passed'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$date=explode(' ',$str);
		if(empty($date[1])) $date[1]='00:00:00';
		$day=get_day($date[0]);
		$date=explode(':',$date[1]);
		$h=$date[0];
		$mi=$date[1];
		return (($day-1)*24+$h)*60+$mi;
};
$cmd['minute_passed'][CMD_STRING]->result=CMD_STRING;
$cmd['minute_passed'][CMD_STRING]->visual=Array('title'=>'кол-во минут от начала отсчёта времени','work_on'=>STRING_DATETIME,'group'=>GROUP_DATE);


$cmd['second_passed'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$date=explode(' ',$str);
		if(empty($date[1])) $date[1]='00:00:00';
		$day=get_day($date[0]);
		$date=explode(':',$date[1]);
		$h=$date[0];
		$mi=$date[1];
		$se=$date[2];
		return ((($day-1)*24+$h)*60+$mi)*60+$se;
};
$cmd['second_passed'][CMD_STRING]->result=CMD_STRING;
$cmd['second_passed'][CMD_STRING]->visual=Array('title'=>'кол-во секунд от начала отсчёта времени','work_on'=>STRING_DATETIME,'group'=>GROUP_DATE);


$cmd['year'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$res=explode('-',$str);
		return $res[0];
};
$cmd['year'][CMD_STRING]->result=CMD_STRING;
$cmd['year'][CMD_STRING]->visual=Array('title'=>'год из даты','work_on'=>STRING_DATE,'group'=>GROUP_DATE);


$cmd['year2'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$date=explode('-',$str);
		$date2=explode('-',date('Y-m-d'));
		$year=$date2[0]-$date[0];
		if($year<0){
			$year=-$year;
			if($date2[1]<$date[1] || ($date2[1]==$date[1] && $date2[2]<$date[2])) $year--; //?
		} else {
			if($date2[1]<$date[1] || ($date2[1]==$date[1] && $date2[2]<$date[2])) $year--;
		}
		$res=$year;
		return $res;
};
$cmd['year2'][CMD_STRING]->result=CMD_STRING;
$cmd['year2'][CMD_STRING]->visual=Array('title'=>'узнать возраст','work_on'=>STRING_DATE,'group'=>GROUP_DATE);


$cmd['gtime'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$res=explode(' ',$str);
		return $res[1];
};
$cmd['gtime'][CMD_STRING]->result=CMD_STRING;
$cmd['gtime'][CMD_STRING]->visual=Array('title'=>'Выдать время из пары дата время','work_on'=>STRING_DATETIME,'result'=>STRING_TIME,'group'=>GROUP_DATE);


$cmd['gdate'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$res=explode(' ',$str);
		return $res[0];
};
$cmd['gdate'][CMD_STRING]->result=CMD_STRING;
$cmd['gdate'][CMD_STRING]->visual=Array('title'=>'Выдать дату из пары дата время','work_on'=>STRING_DATETIME,'result'=>STRING_DATE,'group'=>GROUP_DATE);


$cmd['ghour'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$res=explode(':',$str);
		return $res[0];
};
$cmd['ghour'][CMD_STRING]->result=CMD_STRING;
$cmd['ghour'][CMD_STRING]->visual=Array('title'=>'Выдать часы из пары часы:минуты','work_on'=>STRING_TIME,'group'=>GROUP_DATE);


$cmd['gminute'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$res=explode(':',$str);
		return $res[1];
};
$cmd['gminute'][CMD_STRING]->result=CMD_STRING;
$cmd['gminute'][CMD_STRING]->visual=Array('title'=>'Выдать минуты из пары часы:минуты','work_on'=>STRING_TIME,'group'=>GROUP_DATE);


$cmd['uclose'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if($str!='' && $str[strlen($str)-1]!='/') $str.='/';
		return $str;
};
$cmd['uclose'][CMD_STRING]->result=CMD_STRING;
$cmd['uclose'][CMD_STRING]->visual=Array('title'=>'Закрыть не пустой URL слешем','work_on'=>STRING_URL,'group'=>GROUP_STR_URL);


$cmd['uopen'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if($str!='' && $str[0]!='/') $str='/'.$str;
		return $str;
};
$cmd['uopen'][CMD_STRING]->result=CMD_STRING;
$cmd['uopen'][CMD_STRING]->visual=Array('title'=>'Добавить в начало не пустого URL слеш','work_on'=>STRING_URL,'group'=>GROUP_STR_URL);


$cmd['uboth'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if($str!='' && $str[0]!='/') $str='/'.$str;
		if($str!='' && $str[strlen($str)-1]!='/') $str.='/';
		return $str;
};
$cmd['uboth'][CMD_STRING]->result=CMD_STRING;
$cmd['uboth'][CMD_STRING]->visual=Array('title'=>'Обрамить не пустой URL слешами','work_on'=>STRING_URL,'group'=>GROUP_STR_URL);


$cmd['rfc822'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$str=explode('-',$str);
		if(count($str)==3){
			$str=mktime(0,0,0,$str[1],$str[2],$str[0]);
			$res=date('r',$str);
		} else $res=date('r',mktime(0,0,0,date('m'),date('d'),date('Y')));
		return $res;
};
$cmd['rfc822'][CMD_STRING]->result=CMD_STRING;
$cmd['rfc822'][CMD_STRING]->visual=Array('title'=>'Выдать дату в формате RFC822','group'=>GROUP_STR_SPEC);


$cmd['ds'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$str=str_replace(chr(13),' ',$str);
		$str=str_replace(chr(10),' ',$str);
		$str=str_replace('&nbsp;',' ',$str);
		$str=str_replace('	',' ',$str);
		while(strstr5($str,'  ')) $str=str_replace('  ',' ',$str);
		return $str;
};
$cmd['ds'][CMD_STRING]->result=CMD_STRING;
$cmd['ds'][CMD_STRING]->visual=Array('title'=>'Удалить повторяющиеся пробелы (с переносами строк)','group'=>GROUP_TEXT);


$cmd['dt'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$str=str_replace('&nbsp;',' ',$str);
		$str=str_replace('	',' ',$str);
		while(strstr5($str,'  ')) $str=str_replace('  ',' ',$str);
		return $str;
};
$cmd['dt'][CMD_STRING]->result=CMD_STRING;
$cmd['ds2'][CMD_STRING]->alias=&$cmd['dt'][CMD_STRING];
$cmd['dt'][CMD_STRING]->visual=Array('title'=>'Удалить повторяющиеся пробелы (без переносов строк)','group'=>GROUP_TEXT);


$cmd['safetpl'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$str=str_replace('[','[[',$str);
		$str=str_replace(']',']]',$str);
		return $str;
};
$cmd['safetpl'][CMD_STRING]->result=CMD_STRING;
$cmd['safetpl'][CMD_STRING]->visual=Array('title'=>'Обезопасить текст для последующей шаблонизации','group'=>GROUP_STR_TPL);


$cmd['latinica'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return latinica($str);
};
$cmd['latinica'][CMD_STRING]->result=CMD_STRING;
$cmd['latinica'][CMD_STRING]->visual=Array('title'=>'Латинизировать текст','group'=>GROUP_STR_SPEC);


$cmd['longfix'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return longfix($str);
};
$cmd['longfix'][CMD_STRING]->result=CMD_STRING;
$cmd['longfix'][CMD_STRING]->visual=Array('title'=>'Разбить длинные слова в тексте','group'=>GROUP_TEXT);


$cmd['num?'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return is_num($str);
};
$cmd['num?'][CMD_STRING]->result=CMD_STRING;
$cmd['num?'][CMD_STRING]->visual=Array('title'=>'Это число?','group'=>GROUP_NUM,'work_on'=>STRING_NUM);


$cmd['cascade'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if(!empty($op->put)) $step=parse_var($op->put); else $step=0;
		return cascading_html($str,$step);
};
$cmd['cascade'][CMD_STRING]->result=CMD_STRING;
$cmd['cascade'][CMD_STRING]->visual=Array('title'=>'Оформить HTML в виде каскада','group'=>GROUP_HTML,'put'=>Array(
		0=>Array('title'=>'Начальный сдвиг (кол-во табов)','type'=>CMD_STRING,'req'=>0)
));


$cmd['valid_email'][CMD_STRING]->process=function(&$str,&$op,&$ops){
	return filter_var($str, FILTER_VALIDATE_EMAIL);
};
$cmd['valid_email'][CMD_STRING]->result=CMD_STRING;
$cmd['valid_email'][CMD_STRING]->visual=Array('title'=>'Проверка email-а','group'=>GROUP_STR_SPEC);


$cmd['copy'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		if(!empty($op->put[1])){
			$v1=parse_var($op->put[0]);
			$v2=parse_var($op->put[1]);
		} else {
			$v1=parse_var($op->put[0]);
			$v2=mb_strlen($str,'UTF-8')/*-$v1+1*/;//+1 ? //??
		}
		return mb_substr($str,$v1,$v2,'UTF-8');
};
$cmd['copy'][CMD_STRING]->result=CMD_STRING;
$cmd['copy2'][CMD_STRING]->alias=&$cmd['copy'][CMD_STRING];
$cmd['substr'][CMD_STRING]->alias=&$cmd['copy'][CMD_STRING];
$cmd['copy'][CMD_STRING]->visual=Array('title'=>'Копировать','put'=>Array(
		0=>Array('title'=>'Начальная позиция','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Длинна','type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_TEXT);


$cmd['search'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$v=parse_var($op->put);
		if($str=='' || $v=='') $res=''; else $res=strpos($str,$v);
		if($res===false){ $res=-2; }
		return $res;
};
$cmd['search'][CMD_STRING]->result=CMD_STRING;
$cmd['search'][CMD_STRING]->visual=Array('title'=>'Позиция найденного материала в тексте','put'=>Array(
		0=>Array('title'=>'Искомое','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_SEARCH);


$cmd['search_count'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$v=parse_var($op->put);
		if($str=='' || $v=='') return 0;
		return substr_count($str,$v);
};
$cmd['search_count'][CMD_STRING]->result=CMD_STRING;
$cmd['search_count'][CMD_STRING]->visual=Array('title'=>'Количество нахождений материала в тексте','put'=>Array(
		0=>Array('title'=>'Искомое','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_SEARCH);


$cmd['plagiat'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$v=parse_var($op->put);
		return plagiat($str,$v);
};
$cmd['plagiat'][CMD_STRING]->result=CMD_STRING;
$cmd['plagiat'][CMD_STRING]->visual=Array('title'=>'Оценка схожести текстов','put'=>Array(
		0=>Array('title'=>'Текст для сравнения','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_SEARCH);


$cmd['format_zero'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$v=parse_var($op->put);
		$res=$str;
		while(strlen($res)<$v) $res='0'.$res;
		return $res;
};
$cmd['format_zero'][CMD_STRING]->result=CMD_STRING;
$cmd['format_zero'][CMD_STRING]->visual=Array('title'=>'Добавить нули слева','put'=>Array(
		0=>Array('title'=>'Количество нулей','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_NUM);


$cmd['char'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$v=parse_var($op->put);
		if($str=='' || $v=='') $res=''; else $res=mb_substr($str,$v-1,1,'utf8');
		return $res;
};
$cmd['char'][CMD_STRING]->result=CMD_STRING;
$cmd['char'][CMD_STRING]->visual=Array('title'=>'Выдать символ','hidden'=>1,'put'=>Array(
		0=>Array('title'=>'Номер символа (начинается с 1)','type'=>CMD_STRING,'req'=>1)
));


$cmd['dbl'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$v=(int)parse_var($op->put);
		$res='';
		if($v==0) $res=''; else for($z=0;$z<$v;$z++) $res=$res.$str;
		return $res;
};
$cmd['dbl'][CMD_STRING]->result=CMD_STRING;
$cmd['dbl'][CMD_STRING]->visual=Array('title'=>'Дублировать строку','put'=>Array(
		0=>Array('title'=>'Количество повторов','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_TEXT);


$cmd['contain'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$v=parse_var($op->put);		
		if($str=='' || $v=='') $res=false; else {
			$res=strpos($str,$v);
			if($res===false) $res=false; else $res=true;
		}
		return $res;
};
$cmd['contain'][CMD_STRING]->result=CMD_STRING;
$cmd['contain'][CMD_STRING]->visual=Array('title'=>'Проверка на содержание материала в тексте','put'=>Array(
		0=>Array('title'=>'Искомое','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_SEARCH);


$cmd['scopy'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$v[1]=parse_var($op->put);
		$str=explode(' ',$str);
		$res='';
		for($i=0;$i<count($str);$i++){
			if(!empty($res)) $res.=' ';
			$res.=$str[$i];
			if(mb_strlen($res,'UTF-8')>$v[1]) break;
		}
		return $res;
};
$cmd['scopy'][CMD_STRING]->result=CMD_STRING;
$cmd['scopy'][CMD_STRING]->visual=Array('title'=>'Обрезать строку не дробя слова','put'=>Array(
		0=>Array('title'=>'Длинна','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_TEXT);


$cmd['scopy_stop'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if($op->data=='scopy2' || $op->data=='scopy_stop') $usl=1;
		else if($op->data=='scopy3' || $op->data=='scopy_point_stop') $usl=2;
		$v1=parse_var($op->put[0]);
		$v2=parse_var($op->put[1]);
		$str=explode(' ',$str);
		$res='';
		for($i=0;$i<count($str);$i++){
			if(!empty($res)) $res.=' ';
			$res.=$str[$i];
			if($usl==1) {if(mb_strlen($res,'UTF-8')>$v1 || strstr5($res,$v2)) break;}
			else if($usl==2)  {if(mb_strlen($res,'UTF-8')>=$v1 && strstr5($str[$i],$v2)) break;}
		}
		return $res;
};
$cmd['scopy_stop'][CMD_STRING]->result=CMD_STRING;
$cmd['scopy3'][CMD_STRING]->alias=&$cmd['scopy_stop'][CMD_STRING];
$cmd['scopy2'][CMD_STRING]->alias=&$cmd['scopy_stop'][CMD_STRING];
$cmd['scopy_point_stop'][CMD_STRING]->alias=&$cmd['scopy_stop'][CMD_STRING];
$cmd['scopy_stop'][CMD_STRING]->visual=Array('title'=>'Обрезать строку не дробя слова (с триггером остановки)','put'=>Array(
		0=>Array('title'=>'Длинна','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Триггер остановки','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_TEXT);
$cmd['scopy_point_stop'][CMD_STRING]->visual=Array('title'=>'Обрезать строку не дробя слова (с триггером остановки, срабатывающим при превышении длинны)','put'=>Array(
		0=>Array('title'=>'Длинна','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Триггер остановки (срабатывает при превышении длинны)','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_TEXT);


$cmd['replace'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$v1=parse_var($op->put[0]);
		$v2=parse_var($op->put[1]);
		return str_replace($v1,$v2,$str);
};
$cmd['replace'][CMD_STRING]->result=CMD_STRING;
$cmd['replace'][CMD_STRING]->visual=Array('title'=>'Замена','put'=>Array(
		0=>Array('title'=>'Что','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Чем','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_TEXT);


$cmd['explode'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return explode(parse_var($op->put),$str);
};
$cmd['explode'][CMD_STRING]->result=CMD_ARRAY;
$cmd['explode'][CMD_STRING]->result_long=CMD_STRING;
$cmd['exp'][CMD_STRING]->alias=&$cmd['explode'][CMD_STRING];
$cmd['exp2'][CMD_STRING]->alias=&$cmd['explode'][CMD_STRING];
$cmd['explode'][CMD_STRING]->visual=Array('title'=>'Разбить строку','put'=>Array(
		0=>Array('title'=>'Чем','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_TEXT);


$cmd['split'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return explodeA(parse_var($op->put),$str,'','',1);
};
$cmd['split'][CMD_STRING]->result=CMD_ARRAY;
$cmd['exp1'][CMD_STRING]->alias=&$cmd['split'][CMD_STRING];
$cmd['split'][CMD_STRING]->visual=Array('title'=>'Разбить строку на две части','put'=>Array(
		0=>Array('title'=>'Чем','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_TEXT);


$cmd['fdate'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$v=parse_var($op->put);
		$str=explode('-',$str);
		if(count($str)<3) $res=$str;
		else $res=date($v, mktime(0, 0, 0, $str[1], $str[2], $str[0]));
		return $res;
};
$cmd['fdate'][CMD_STRING]->result=CMD_STRING;
$cmd['fdate'][CMD_STRING]->visual=Array('title'=>'Форматировать дату','work_on'=>STRING_DATE,'put'=>Array(
		0=>Array('title'=>'Формат','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_DATE);


$cmd['strnum'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$v[0]=parse_var($op->put[0]);
		$v[1]=parse_var($op->put[1]);
		$v[2]=parse_var($op->put[2]);
		if($str=='') $res=$v[2]; else
		return get_str_num($str,$v[0],$v[1],$v[2]);
};
$cmd['strnum'][CMD_STRING]->result=CMD_STRING;
$cmd['strnum'][CMD_STRING]->visual=Array('title'=>'Выдать правильный вариант текста, соотносящегося с числом','work_on'=>STRING_NUM,'put'=>Array(
		0=>Array('title'=>'Один','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Два-четыре','type'=>CMD_STRING,'req'=>1),
		2=>Array('title'=>'Пять-девять','type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_NUM);


$cmd['autoclose'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return safe_content($str);
};
$cmd['autoclose'][CMD_STRING]->result=CMD_STRING;
$cmd['autoclose'][CMD_STRING]->visual=Array('title'=>'Закрыть открытые HTML теги','group'=>GROUP_HTML);


$cmd['shurl'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return enshurl($str);
};
$cmd['shurl'][CMD_STRING]->result=CMD_STRING;
$cmd['shurl'][CMD_STRING]->visual=Array('title'=>'Кодировать URL (указывающий на домен, привязанный к текущей установке системы)','work_on'=>STRING_URL,'group'=>GROUP_STR_URL);


$cmd['num'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return get_num($str);
};
$cmd['num'][CMD_STRING]->result=CMD_STRING;
$cmd['num'][CMD_STRING]->visual=Array('title'=>'число прописью','work_on'=>STRING_NUM,'group'=>GROUP_NUM);


$cmd['astext'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return as_html($str);
};
$cmd['astext'][CMD_STRING]->result=CMD_STRING;
$cmd['astext'][CMD_STRING]->visual=Array('title'=>'Выдать HTML как текст (экранирование HTML)','group'=>GROUP_STR_SPEC);


$cmd['safe_input'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return htmlspecialchars($str);
};
$cmd['safe_input'][CMD_STRING]->result=CMD_STRING;
$cmd['safe'][CMD_STRING]->alias=&$cmd['safe_input'][CMD_STRING];
$cmd['safe_input'][CMD_STRING]->visual=Array('title'=>'Очистка содержимого от HTML тегов (экранирование)','group'=>GROUP_HTML);


$cmd['quotes'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return comillas($str);
};
$cmd['quotes'][CMD_STRING]->result=CMD_STRING;
$cmd['quotes'][CMD_STRING]->visual=Array('title'=>'Преобразование двойных кавычек в типографические ёлочки','group'=>GROUP_STR_SPEC);


$cmd['e5row'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if(is_object($str) && isset($str->id)) $str=$str->id;
		if(is_numeric($str)){
			global $rlink;
			$res=$str;
			seek_rlink($res);
			if(isset($rlink[$res]) && check_row($res,0,0,"view",$rlink[$res]->user,$rlink[$res]->users)){
				return $rlink[$res];
			}
		}
		return false;
};
$cmd['e5row'][CMD_STRING]->result=CMD_ROW;
$cmd['e5row'][CMD_STRING]->visual=Array('title'=>'Найти объект по данному ID','group'=>GROUP_STR_SPEC);


$cmd['iconv'][CMD_STRING]->process=function(&$str,&$op,&$ops){		
		$from=parse_var($op->put[0]);
		$to=parse_var($op->put[1]);
		return iconv($from,$to,$str);
};
$cmd['iconv'][CMD_STRING]->result=CMD_STRING;
$cmd['iconv'][CMD_STRING]->visual=Array('title'=>'Сменить кодировку','put'=>Array(
		0=>Array('title'=>'Исходная кодировка','type'=>CMD_STRING,'req'=>1,'default'=>'windows-1251'),
		1=>Array('title'=>'Кодировать в кодировку:','type'=>CMD_STRING,'req'=>1,'default'=>'utf-8')
),'group'=>GROUP_STR_SPEC);


$cmd['json_encode'][CMD_STRING]->process=function(&$str,&$op,&$ops){		
		return json_encode($str);
};
$cmd['json_encode'][CMD_STRING]->result=CMD_STRING;
$cmd['json_encode'][CMD_STRING]->visual=Array('title'=>'Кодировать в JSON','group'=>GROUP_STR_SPEC);


$cmd['json_decode'][CMD_STRING]->process=function(&$str,&$op,&$ops){		
		return json_decode($str);
};
$cmd['json_decode'][CMD_STRING]->result=CMD_MIXED;
$cmd['json_decode'][CMD_STRING]->visual=Array('title'=>'Раскодировать JSON','group'=>GROUP_STR_SPEC);


$cmd['serialize'][CMD_STRING]->process=function(&$str,&$op,&$ops){		
		return serialize($str);
};
$cmd['serialize'][CMD_STRING]->result=CMD_STRING;
$cmd['serialize'][CMD_STRING]->visual=Array('title'=>'Кодировать в Serialize','group'=>GROUP_STR_SPEC);


$cmd['unserialize'][CMD_STRING]->process=function(&$str,&$op,&$ops){		
		return unserialize($str);
};
$cmd['unserialize'][CMD_STRING]->result=CMD_MIXED;
$cmd['unserialize'][CMD_STRING]->visual=Array('title'=>'Раскодировать Serialize','group'=>GROUP_STR_SPEC);


$cmd['dump'][CMD_STRING]->process=function(&$str,&$op,&$ops){		
		$t=start_buffer();
		var_dump($str);
		return return_buffer($t);
};
$cmd['dump'][CMD_STRING]->result=CMD_STRING;
$cmd['dump'][CMD_STRING]->visual=Array('title'=>'Вывести дамп переменной','hidden'=>1);


$cmd['with'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		$res=backup_globals();
		$GLOBALS['for_value']=$str;
		if(!isset($GLOBALS["spec_step"])) $GLOBALS["spec_step"]=0;
		if(!isset($GLOBALS["spec_step2"])) $GLOBALS["spec_step2"]=0;
		$GLOBALS["spec_step"]++;
		$GLOBALS["spec_step2"]++;			
		$r=parse_var($op->put);
		return_globals($res);
		return $r;
};
$cmd['with'][CMD_STRING]->result=CMD_MIXED;
$cmd['with'][CMD_STRING]->visual=Array('title'=>'Исполнить оператор','put'=>Array(
		0=>Array('title'=>'оператор','type'=>CMD_CMD,'req'=>1)
),'group'=>GROUP_STR_SPEC);


// Математические функции


$cmd['rand'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return rand(1,$str);
};
$cmd['rand'][CMD_STRING]->result=CMD_STRING;
$cmd['rand'][CMD_STRING]->visual=Array('title'=>'Случайное число (между еденицей и текущим)','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['abs'][CMD_STRING]->process=function(&$str,&$op,&$ops){		
		return abs($str);
};
$cmd['abs'][CMD_STRING]->result=CMD_STRING;
$cmd['abs'][CMD_STRING]->visual=Array('title'=>'Модуль числа','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['round'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return round($str);
};
$cmd['round'][CMD_STRING]->result=CMD_STRING;
$cmd['round'][CMD_STRING]->visual=Array('title'=>'округлить','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['floor'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if(empty($op->put)){
			return floor($str);
		} else {
			return flooring($str,parse_var($op->put));
		}
};
$cmd['floor'][CMD_STRING]->result=CMD_STRING;
$cmd['floor'][CMD_STRING]->visual=Array('title'=>'округлить в меньшую сторону','put'=>Array(
		0=>Array('title'=>'целое должно быть кратно этому числу (можно писать дробные значения)','type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['ceil'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if(empty($op->put)){
			return ceil($str);
		} else {
			return ceiling($str,parse_var($op->put));
		}	
};
$cmd['ceil'][CMD_STRING]->result=CMD_STRING;
$cmd['ceil'][CMD_STRING]->visual=Array('title'=>'округлить в большую сторону','put'=>Array(
		0=>Array('title'=>'целое должно быть кратно этому числу (можно писать дробные значения)','type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['acos'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return acos($str);
};
$cmd['acos'][CMD_STRING]->result=CMD_STRING;
$cmd['acos'][CMD_STRING]->visual=Array('title'=>'Арккосинус','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['cos'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return cos($str);
};
$cmd['cos'][CMD_STRING]->result=CMD_STRING;
$cmd['cos'][CMD_STRING]->visual=Array('title'=>'Косинус','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['asin'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return asin($str);
};
$cmd['asin'][CMD_STRING]->result=CMD_STRING;
$cmd['asin'][CMD_STRING]->visual=Array('title'=>'Арксинус','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['sin'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return sin($str);
};
$cmd['sin'][CMD_STRING]->result=CMD_STRING;
$cmd['sin'][CMD_STRING]->visual=Array('title'=>'Синус','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['atan'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return atan($str);
};
$cmd['atan'][CMD_STRING]->result=CMD_STRING;
$cmd['atan'][CMD_STRING]->visual=Array('title'=>'Арктангенс','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['tan'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return tan($str);
};
$cmd['tan'][CMD_STRING]->result=CMD_STRING;
$cmd['tan'][CMD_STRING]->visual=Array('title'=>'Тангенс','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['log'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		if(empty($op->put)){
			return log($str,parse_var($op->put));
		} else return log($str);
};
$cmd['log'][CMD_STRING]->result=CMD_STRING;
$cmd['log'][CMD_STRING]->visual=Array('title'=>'Логарифм','put'=>Array(
		0=>Array('title'=>'основание логарифма (если не указано - вернёт натуральный логарифм)','default'=>10,'type'=>CMD_STRING,'req'=>0)
),'group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['sqrt'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return sqrt($str);
};
$cmd['sqrt'][CMD_STRING]->result=CMD_STRING;
$cmd['sqrt'][CMD_STRING]->visual=Array('title'=>'Квадратный корень','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['pow'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return pow($str,parse_var($op->put));
};
$cmd['pow'][CMD_STRING]->result=CMD_STRING;
$cmd['pow'][CMD_STRING]->visual=Array('title'=>'Степень','put'=>Array(
		0=>Array('title'=>'Возведение в степень','default'=>2,'type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['hexdec'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return hexdec($str);
};
$cmd['hexdec'][CMD_STRING]->result=CMD_STRING;
$cmd['hexdec'][CMD_STRING]->visual=Array('title'=>'Шестнадцатиричное в десятичную','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['dechex'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return dechex($str);
};
$cmd['dechex'][CMD_STRING]->result=CMD_STRING;
$cmd['dechex'][CMD_STRING]->visual=Array('title'=>'Десятичное в шестнадцатеричное','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['deg2rad'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return deg2rad($str);
};
$cmd['deg2rad'][CMD_STRING]->result=CMD_STRING;
$cmd['deg2rad'][CMD_STRING]->visual=Array('title'=>'Градусы в радианы','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['rad2deg'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return rad2deg($str);
};
$cmd['rad2deg'][CMD_STRING]->result=CMD_STRING;
$cmd['rad2deg'][CMD_STRING]->visual=Array('title'=>'Радианы в градусы','group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


$cmd['base_convert'][CMD_STRING]->process=function(&$str,&$op,&$ops){
		return base_convert($str,parse_var($op->put[0]),parse_var($op->put[1]));
};
$cmd['base_convert'][CMD_STRING]->result=CMD_STRING;
$cmd['base_convert'][CMD_STRING]->visual=Array('title'=>'Преобразование между системами счисления','put'=>Array(
		0=>Array('title'=>'Исходная','default'=>10,'type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Конечная','default'=>2,'type'=>CMD_STRING,'req'=>1)
),'group'=>GROUP_MATH,'work_on'=>STRING_NUM,'result'=>STRING_NUM);


?>