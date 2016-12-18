<?php

if(!isset($_GET["id"]) && !isset($_POST["id"])) {include('main.php'); exit;}
global $user;
$madd2='';$madd3='';
prep_tables();
global $stables;
if(empty($fmod) && !empty($_POST["fmod"])) $fmod=$_POST["fmod"];
if(empty($fmod) && !empty($_GET["fmod"])) $fmod=$_GET["fmod"];
if(!empty($fmod)){
	$madd2='<input type="hidden" name="fmod" value="'.$fmod.'">';
	$madd3='?fmod='.$fmod;
}
perm_folder();
define_lng('perm');

if($id!=0){
	getrow($db,"SELECT * FROM main_auth WHERE auth_id=$id");
	if(!empty($db->Record)) foreach($db->Record AS $var=>$value)$$var=$value;
}
if($user->super==0 && $id==0 && !empty($fmod)){
	if(!check_mod($fmod,'edit')) {include('main.php'); exit;}
	$group_module=$fmod;
} else if($user->super==0){
	if($id==0){include('main.php'); exit;}
	if($auth_type==0) $c=check_user(-$id,'rules',$auth_owner);
	else $c=check_group($id,'rules');
	if(!$c){include('main.php'); exit;}
}

if(empty($id) && !empty($fmod)) $group_module=$fmod;

if(!isset($_GET["id"]) && !isset($_POST["id"])) {include('main.php'); exit;}

$vars=Array('url'=>'group'.$madd3);
$vars['go_back_text']=lng('Back to management of groups');
echo shell_tpl_admin('block/go_back_box',$vars);

if(!empty($action) && $action=='edit' && check_form_protection_key($_POST['key'],'perm',1)){
	if(!empty($fmod)) update_module_state($fmod);
	$t=explode('&&',$_POST["collect"]);
	foreach($t AS $tt){
		$ttt=explodeA('=',$tt,'','',1);
		$_POST[$ttt[0]]=$ttt[1];
	}
	unset($_POST["collect"]);
	$perm=Array();$perm2=Array();
	$br=false;
	$z=spdecode($_POST["spec"],'spec');if($z==0) $br=true;
	global $sucdec;
	$sucdec=1;//4;// почему значение увеличилось на 3 я не понимаю!
	foreach($_POST AS $var=>$value)if(strstr($var,':')){
		$value=spdecode($value,$var);
		$var=explode(':',$var);
		if(count($var)==3) $t=&$perm[$var[0]][$var[1]];
		if(count($var)==4){
			$t=&$perm2[$var[0]][$var[1]][$var[2]];
		}
		$n=$var[count($var)-1];
		if(!is_object($t)) $t=new perm();
		if(empty($value)) $value='0';
		$value=(int)$value;
		if($n=='view') $t->view=$value;
		if($n=='add') $t->add=$value;
		if($n=='edit') $t->edit=$value;
		if($n=='del') $t->del=$value;
		if($n=='upload') $t->upload=$value;
		if($n=='invite') $t->invite=$value;
		if($n=='leave') $t->leave=$value;
		if($n=='rules') $t->rules=$value;
		if($n=='reg') $t->reg=$value;
	}
	//echo $sucdec.' '.$z;
	if($sucdec!=$z && $sucdec+3!=$z) $br=true;//Что за косяк с +3 я так и не понял. вполне возможно что после это вылезет в какой-нибудь +6
	if(!$br){
		if($id!=0 || empty($fmod)){
			$db->query("DELETE FROM auth_perm WHERE perm_auth=$id",3,"auth_perm");
			if(isset($_POST["auth_folder2"])){
				if(check_folder($_POST["auth_folder2"],'view') || (spdecode($_POST["auth_folder2"],'auth_folder2')!=0)){
						$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_folder)
									VALUES($id, 6, 5, '".spdecode($_POST["auth_folder2"],'auth_folder2')."')",3,"auth_perm");
				}
			}
		} else {
			$id=0;
			del_current_perm(0,$fmod);
		}
		foreach($perm AS $var=>$val){
			if($var==0) foreach($val AS $var2=>$val2){
				if($var2==0){
					if($val2->view!=-1 || $val2->edit!=-1 || $val2->add!=-1 || $val2->del!=-1 || $val2->rules!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_edit, perm_add, perm_del, perm_rules)
								VALUES($id, 0, 0, 0, $val2->view, $val2->edit, $val2->add, $val2->del, $val2->rules)",3,"auth_perm");
				} else {
					if($val2->view!=-1 || $val2->edit!=-1 || $val2->rules!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_edit, perm_rules)
								VALUES($id, 1, 0, $var2, $val2->view, $val2->edit, $val2->rules)",3,"auth_perm");
				}
			}
			if($var==1) foreach($val AS $var2=>$val2){
				if($var2==0){
					if($val2->view!=-1 || $val2->edit!=-1 || $val2->add!=-1 || $val2->del!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_edit, perm_add, perm_del)
								VALUES($id, 0, 1, 0, $val2->view, $val2->edit, $val2->add, $val2->del)",3,"auth_perm");
				} else {
					if($val2->view!=-1 || $val2->edit!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_edit)
								VALUES($id, 1, 1, $var2, $val2->view, $val2->edit)",3,"auth_perm");
				}
			}
			if($var=='4a') foreach($val AS $var2=>$val2){
				if($val2->view!=-1 || $val2->edit!=-1 || $val2->add!=-1 || $val2->del!=-1)
				$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_edit, perm_add, perm_del)
							VALUES($id, 3, 4, $var2, $val2->view, $val2->edit, $val2->add, $val2->del)",3,"auth_perm");		
			}
			if($var=='4b') foreach($val AS $var2=>$val2){
				if($val2->view!=-1 || $val2->edit!=-1 || $val2->add!=-1 || $val2->del!=-1)
				$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_edit, perm_add, perm_del)
							VALUES($id, 4, 4, $var2, $val2->view, $val2->edit, $val2->add, $val2->del)",3,"auth_perm");		
			}				
			if($var=='4c') foreach($val AS $var2=>$val2){
					if($val2->view!=-1 || $val2->edit!=-1 || $val2->add!=-1 || $val2->del!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_edit, perm_add, perm_del)
								VALUES($id, 0, 4, 0, $val2->view, $val2->edit, $val2->add, $val2->del)",3,"auth_perm");
			}
			if($var=='4d') foreach($val AS $var2=>$val2){
					if($val2->view!=-1 || $val2->edit!=-1 || $val2->add!=-1 || $val2->del!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_edit, perm_add, perm_del)
								VALUES($id, 5, 4, $var2, $val2->view, $val2->edit, $val2->add, $val2->del)",3,"auth_perm");
			}
			if($var=='4e') foreach($val AS $var2=>$val2){
					if($val2->view!=-1 || $val2->edit!=-1 || $val2->add!=-1 || $val2->del!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_subtable, perm_view, perm_edit, perm_add, perm_del)
								VALUES($id, 7, 4, '$var2', $val2->view, $val2->edit, $val2->add, $val2->del)",3,"auth_perm");
			}
			if($var=='3e') foreach($val AS $var2=>$val2){
					if($val2->view!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_subtable, perm_view)
								VALUES($id, 7, 3, '$var2', $val2->view)",3,"auth_perm");
			}
			if($var=='5') foreach($val AS $var2=>$val2){
				if($var2=='0'){
					if($val2->view!=-1 || $val2->edit!=-1 || $val2->del!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_folder, perm_view, perm_del, perm_edit)
								VALUES($id, 0, 5, '0', $val2->view, $val2->del, $val2->edit)",3,"auth_perm");
				} else {
					if($val2->view!=-1 || $val2->edit!=-1 || $val2->del!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_folder, perm_view, perm_del, perm_edit)
								VALUES($id, 1, 5, '$var2', $val2->view,  $val2->del, $val2->edit)",3,"auth_perm");
				}
			}
			if($var=='6') foreach($val AS $var2=>$val2){
				if($var2==0){
					if($val2->view!=-1 || $val2->rules!=-1 || $val2->del!=-1 || $val2->reg!=-1 || $val2->edit!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_rules, perm_del, perm_reg, perm_edit)
								VALUES($id, 0, 6, 0, $val2->view, $val2->rules, $val2->del, $val2->reg, $val2->edit)",3,"auth_perm");
				} else {
					if($val2->view!=-1 || $val2->rules!=-1 || $val2->del!=-1 || $val2->edit!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_rules, perm_del, perm_edit)
								VALUES($id, 1, 6, $var2, $val2->view, $val2->rules, $val2->del, $val2->edit)",3,"auth_perm");
				}
			}
			if($var=='7') foreach($val AS $var2=>$val2){
					if($val2->view!=-1 || $val2->edit!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_edit)
								VALUES($id, 1, 7, $var2, $val2->view, $val2->edit)",3,"auth_perm");
			}
			if($var=='8') foreach($val AS $var2=>$val2){
				if($var2==0){
					if($val2->view!=-1 || $val2->add!=-1 || $val2->edit!=-1 || $val2->del!=-1 || $val2->rules!=-1 || $val2->invite!=-1 || $val2->leave!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_add, perm_edit, perm_del, perm_rules, perm_invite, perm_leave)
								VALUES($id, 0, 8, 0, $val2->view, $val2->add, $val2->edit, $val2->del, $val2->rules, $val2->invite, $val2->leave)",3,"auth_perm");
				} else {
					if($val2->view!=-1 || $val2->invite!=-1 || $val2->leave!=-1 || $val2->rules!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_invite, perm_leave, perm_rules)
								VALUES($id, 1, 8, $var2, $val2->view, $val2->invite, $val2->leave, $val2->rules)",3,"auth_perm");
				}
			}
		}
		if(!empty($perm2)) foreach($perm2 AS $var=>$val){
			if($var==3) foreach($val AS $var2=>$val2){
				if($var2==0){
					foreach($val2 AS $var3=>$val3){
						if($val3->view!=-1)
						$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view)
									VALUES($id, 0, 3, 0, $val3->view)",3,"auth_perm");
					}
				} else foreach($val2 AS $var3=>$val3){
					if($var3==0){
						if($val3->view!=-1)
						$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view)
									VALUES($id, 3, 3, $var2, $val3->view)",3,"auth_perm");
					} else {
						if($val3->view!=-1)
						$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view)
									VALUES($id, 1, 3, $var3, $val3->view)",3,"auth_perm");
					}
				}
			}
			if($var==2) foreach($val AS $var2=>$val2){
				if($var2==0){
					foreach($val2 AS $var3=>$val3){
						if($val3->view!=-1 || $val3->edit!=-1 || $val3->add!=-1 || $val3->del!=-1)
						$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_edit, perm_add, perm_del)
									VALUES($id, 0, 2, 0, $val3->view, $val3->edit, $val3->add, $val3->del)",3,"auth_perm");
					}
				} else foreach($val2 AS $var3=>$val3){
					if($var3==0){
						if($val3->view!=-1 || $val3->edit!=-1 || $val3->add!=-1 || $val3->del!=-1)
						$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_edit, perm_add, perm_del)
									VALUES($id, 3, 2, $var2, $val3->view, $val3->edit, $val3->add, $val3->del)",3,"auth_perm");
					} else {
						if($val3->view!=-1 || $val3->edit!=-1)
						$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_edit)
									VALUES($id, 1, 2, $var3, $val3->view, $val3->edit)",3,"auth_perm");
					}
				}
			}
			if($var=='6a') foreach($val AS $var2=>$val2){
				foreach($val2 AS $var3=>$val3){
					if($val3->view!=-1 || $val3->rules!=-1 || $val3->del!=-1 || $val3->reg!=-1 || $val3->edit!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_rules, perm_del, perm_reg, perm_edit)
								VALUES($id, 3, 6, $var3, $val3->view, $val3->rules, $val3->del, $val3->reg, $val3->edit)",3,"auth_perm");
				}
			}
			if($var=='8a') foreach($val AS $var2=>$val2){
				foreach($val2 AS $var3=>$val3){
					if($val3->view!=-1 || $val3->add!=-1 || $val3->edit!=-1 || $val3->del!=-1 || $val3->rules!=-1 || $val3->invite!=-1 || $val3->leave!=-1)
					$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_view, perm_add, perm_edit, perm_del, perm_rules, perm_invite, perm_leave)
								VALUES($id, 3, 8, $var3, $val3->view, $val3->add, $val3->edit, $val3->del, $val3->rules, $val3->invite, $val3->leave)",3,"auth_perm");
				}
			}
		}
		$vars['title']=lng('Accesses successfully modified');
		echo shell_tpl_admin('block/message_box',$vars);
	} else {
		$vars['title']=lng('Access does not modified');
		$vars['msg']=lng('Protection error');
		echo shell_tpl_admin('block/message_box',$vars);
		//echo '<div style="font-size: 16px; color: #FF0000;" align="center">'.lng('Protection error').'</div>';
	}
}

global $id;
$pauth=Array();
$tmp=getall($db,"SELECT * FROM auth_perm WHERE perm_auth=".$id);
if(!empty($tmp)) foreach($tmp AS $tm) if($tm["perm_type"]!=5 && $tm["perm_target"]!=7)	$pauth[$tm["perm_type"]][$tm["perm_target"]][$tm["perm_object"]]=$tm;
								else if($tm["perm_type"]==5)					$pauth[$tm["perm_type"]][$tm["perm_target"]][$tm["perm_folder"]]=$tm;
								else if($tm["perm_target"]==7)					$pauth[$tm["perm_type"]][$tm["perm_target"]][$tm["perm_subtable"]]=$tm;

function echo_subtables($table,$owners=Array(),$tviss=Array(),$exists=Array()){
	global $stables,$tables,$sep,$tvis,$tvis2,$tvis3,$cbol;
	$exists[$table]=1;
	$tviss[count($tviss)]='none';
	$itvis=count($tviss);
	$res='';
	$nbb=dblchar('&nbsp;&nbsp;',count($owners)-1);
	foreach($stables[$table] AS $tbl)if(empty($exists[$tbl])){
		$owners2=$owners;
		$owners2[]=$tbl;
		$st=implode(',',$owners2);

		$ids='3e:'.$st;
		$rule=get_root3(3,7,$st);		
		
		$ids2='4e:'.$st;
		$rule2=get_root3(4,7,$st);
		$tmp=Array();
		//защита от возможного зацикливания подтаблиц
		if(!empty($stables[$tbl])) foreach($stables[$tbl] AS $tbl2) if(!in_array($tbl2,$owners)) $tmp[]=$tbl;
		$cbol=false;
		if(empty($tmp)) $res2='<tr><td> '.$nbb.'&nbsp;&nbsp;&nbsp;&nbsp;- '.lng('Subtable').' «'.$tables[$tbl]->name.'»</td><td align="right" align="right"><nobr>'.root(lng('View'),$ids.':view',0,$rule->view,check_tbl($st,'view')).$sep.lng('Objects').': '.root(lng('View'),$ids2.':view',2,$rule2->view,check_row_st($st,'view',1)).$sep.root(lng('Adding'),$ids2.':add',0,$rule2->add,check_row_st($st,'add',1)).$sep.root(lng('Modify'),$ids2.':edit',2,$rule2->edit,check_row_st($st,'edit',1)).$sep.root(lng('Deleting'),$ids2.':del',2,$rule2->del,check_row_st($st,'del',1)).'</nobr></td></tr>';
		else {
			$res2='<tr><td> '.$nbb.'&nbsp;&nbsp;&nbsp;&nbsp;';
			$res2.='<span class="link" OnClick="showhide(\'tr4e'.$st.'\');">';
			$res2.='- '.lng('Subtable').' «'.$tables[$tbl]->name.'»';
			$res2.='</span>';
			$res2.='</td><td align="right" align="right"><nobr>'.root(lng('View'),$ids.':view',0,$rule->view,check_tbl($st,'view')).$sep.lng('Objects').': '.root(lng('View'),$ids2.':view',2,$rule2->view,check_row_st($st,'view',1)).$sep.root(lng('Adding'),$ids2.':add',0,$rule2->add,check_row_st($st,'add',1)).$sep.root(lng('Modify'),$ids2.':edit',2,$rule2->edit,check_row_st($st,'edit',1)).$sep.root(lng('Deleting'),$ids2.':del',2,$rule2->del,check_row_st($st,'del',1)).'</nobr></td></tr>';
			$res2.='<tr id="tr4e'.$st.'" style="display: TVIS'.$itvis.';" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
			$cbbol=$cbol;
			$exists2=$exists;
			$exists2[$tbl]=$tbl;
			$res2.=echo_subtables($tbl,$owners2,$tviss,$exists2);
			if(!$cbol && $cbbol) $cbol=true;
			$res2.='</table></td></tr>';
		}
		if($cbol){
			$tvis1='';$tvis2='';$tvis3='';
			foreach($tviss AS $i=>$v) $tviss[$i]='';
		}
		$res2=str_replace('TVIS'.$itvis,$tviss[$itvis-1],$res2);
		$res.=$res2;
	}
	return $res;
}								


/* Представим себе ситуацию, при которой пользователь состоит
в двух группах:
	контент менеджеры
		доступ к зоне "Админка" (чтение)
		доступ к модулю Статьи (чтение)
		доступ к таблице Статьи (чтение, редактирование, добавление)
		запрет к всем экземплярам таблицы Статьи (в принипе это даже не надо потому что ниже идёт соответствующее умолчание)
	дизайнер шаблонов
		доступ к зоне "Админка" (чтение)
		доступ к модулю "Содержание" (чтение)
		доступ к таблице Шаблоны (чтение, редактирование, добавление)
		доступ ко всем экземплярам таблицы Шаблоны
		запрет на все строки таблицы Шаблоны
и имеет собственные полномочия:
	доступ к экземплярам таблицы "Статьи" - СеоФан
	доступ к строкам таблицы Шаблоны - СеоФан
умолчания следующие:
	все зоны - разрешение на чтение (вход), запрет на все остальные дейтсвия
	зона "Админка" - запрет на чтение (вход) и на все остальные действия
	доступ ко всем модулям - запрет на всё
	доступ ко всем экземплярам - запрет на всё
	
таким образом этот пользователь может редактировать и добавлять статьи в раздел СеоФан а также менять шаблон дизайна СеоФан
*/


?>
<script>
function colct(){
	var a=explode('&&',document.getElementById('collect2').value);
	var c='';
	for (var key in a) {
          var val = a[key];
          var tmp=document.getElementById(val).value;
          if(c!="") c=c+'&&';
          c=c+val+'='+tmp;
        }
        document.getElementById('collect').value=c;
        return true;
}
</script>
<?php

/*echo '<form action="perm" method="post" OnSubmit="return colct();">
	<input type="hidden" name="id" value="'.$id.'">
	<input type="hidden" name="action" value="edit">';*/
	
function spdecode($val,$name){
	global $e5uid,$sucdec;
	if(empty($val) || $val[0]!='|') return $val;
	$data=urldecode(substr($val,1));
	$data=decode($data,$e5uid.$name);
	if(!strpos($data,'||')) return 0;
	$data=explode('||',$data);
	if($data[1]!=$name) return 0;
	$sucdec++;
	return $data[2];
}
	
function spcode($val,$name){
	global $e5uid;
	return '|'.urlencode(code(get_code2(rand(10,20)).'||'.$name.'||'.$val.'||'.get_code2(rand(5,10)),$e5uid.$name));
}

$cspec=1;
$inp='';
$inp2='';
$inpa=Array();
function root($title,$name,$type=0,$default=-1,$allow=1){
	global $cbol,$cspec,$inp,$inp2,$inpa;
	if($default!=-1) $cbol=true;
	if($default==-1){ $f='up'; $val=-1; }
	if($default==0){ $f='deny'; $val=0; }
	if($default==1){ $f='allow'; $val=1; }
	if($default==2){ $f='user3'; $val=2; }
	if($default==3){ $f='group'; $val=3; }
	if($default==5){ $f='allow_all'; $val=5; }
	if($allow==0){
		$cspec++;
		$inpa[$name]=spcode($val,$name);
		return $title.'&nbsp;<img id="p-'.$name.'" src="'.$GLOBALS["base_root"].'/files/editor/'.$f.'.gif" align="absmiddle">';
	} else if($allow==2 && $type>1 && $default!=3 && $default!=1){
		if($val==1) $val=2;
		if($val==3) $val=2;
		$inpa[$name]=$val;
		return '<span class="elem" OnClick="JSQ2(\''.$name.'\','.$type.');">'.$title.'&nbsp;<img id="p-'.$name.'" src="'.$GLOBALS["base_root"].'/files/editor/'.$f.'.gif" align="absmiddle"></span>';
	} else if($allow==3 && $type>1 && $default!=1){
		if($val==1) $val=3;
		$inpa[$name]=$val;
		return '<span class="elem" OnClick="JSQ3(\''.$name.'\','.$type.');">'.$title.'&nbsp;<img id="p-'.$name.'" src="'.$GLOBALS["base_root"].'/files/editor/'.$f.'.gif" align="absmiddle"></span>';
	} else {
		$inpa[$name]=$val;
		return '<span class="elem" OnClick="JSQ(\''.$name.'\','.$type.');">'.$title.'&nbsp;<img id="p-'.$name.'" src="'.$GLOBALS["base_root"].'/files/editor/'.$f.'.gif" align="absmiddle"></span>';
	}
}

/*sdefine('td_obj',0);
sdefine('td_perm',1);
$vars['pre_html']='<br>';
$vars['th']=Array(lng('Object'),lng('Access'));*/

$sep='<div class="sp">|</div>';
$content='<table id="records" cellpadding="3" cellspacing="1" class="perm_records">';
$content.='<tr>';
$content.='<th>'.lng('Object').'</th>';
$content.='<th width="100">'.lng('Access').'</th>';
$content.='</tr>';
function get_root($perm_type,$perm_target,$perm_object){
	stf('get_root');
	global $id,$pauth;
	if(!empty($pauth[$perm_type][$perm_target][$perm_object])){
		$t=$pauth[$perm_type][$perm_target][$perm_object];
		$res->add=$t["perm_add"];
		$res->edit=$t["perm_edit"];
		$res->view=$t["perm_view"];
		$res->del=$t["perm_del"];
		$res->invite=$t["perm_invite"];
		$res->upload=$t["perm_upload"];
		$res->leave=$t["perm_leave"];
		$res->rules=$t["perm_rules"];
		$res->reg=$t["perm_reg"];
	} else {
		$res->add=-1;$res->edit=-1;$res->del=-1;$res->view=-1;$res->upload=-1;$res->invite=-1;$res->leave=-1;$res->rules=-1;$res->reg=-1;
	}
	etf('get_root');
	return $res;
}
function get_root2($perm_type,$perm_target,$perm_folder){
	global $id,$pauth;
	if(!empty($pauth[$perm_type][$perm_target][$perm_folder])){
		$t=$pauth[$perm_type][$perm_target][$perm_folder];
		$res->add=$t["perm_add"];
		$res->edit=$t["perm_edit"];
		$res->view=$t["perm_view"];
		$res->del=$t["perm_del"];
		$res->invite=$t["perm_invite"];
		$res->upload=$t["perm_upload"];
		$res->leave=$t["perm_leave"];
		$res->rules=$t["perm_rules"];
		$res->reg=$t["perm_reg"];
	} else {
		$res->add=-1;$res->edit=-1;$res->del=-1;$res->view=-1;$res->upload=-1;$res->invite=-1;$res->leave=-1;$res->rules=-1;$res->reg=-1;
	}
	return $res;
}
function get_root3($perm_type,$perm_target,$perm_subtable){
	global $id,$pauth;
	if(!empty($pauth[$perm_type][$perm_target][$perm_subtable])){
		$t=$pauth[$perm_type][$perm_target][$perm_subtable];
		$res->add=$t["perm_add"];
		$res->edit=$t["perm_edit"];
		$res->view=$t["perm_view"];
		$res->del=$t["perm_del"];
		$res->invite=$t["perm_invite"];
		$res->upload=$t["perm_upload"];
		$res->leave=$t["perm_leave"];
		$res->rules=$t["perm_rules"];
		$res->reg=$t["perm_reg"];
	} else {
		$res->add=-1;$res->edit=-1;$res->del=-1;$res->view=-1;$res->upload=-1;$res->invite=-1;$res->leave=-1;$res->rules=-1;$res->reg=-1;
	}
	return $res;
}
$rule=get_root(0,0,0);
if(empty($fmod) || $id!=0){
	$t2='<tr><td><span class="link" OnClick="showhide(\'zna\');"><b>'.lng('Sites').'</b></span></td><td align="right"><nobr>'.root(lng('Adding'),'0:0:add',0,$rule->add,check_zone(0,'add')).$sep.root(lng('Deleting'),'0:0:del',0,$rule->del,check_zone(0,'del')).$sep.root(lng('Entrance'),'0:0:view',0,$rule->view,check_zone(0,'view')).$sep.root(lng('Statistics'),'0:0:rules',0,$rule->rules,check_zone(0,'rules')).$sep.root(lng('Control'),'0:0:edit',0,$rule->edit,check_zone(0,'edit')).'</nobr></td></tr>';
	$t2.='<tr id="zna" style="display: TVIS0;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
	global $cbol;
	$zs=getall($db,"SELECT * FROM main_zone WHERE zone_redirect=0 ORDER BY zone_name",1,"main_zone");
	$tvis0='none';
	if(!empty($zs)) foreach($zs AS $z){
		$ids='0:'.$z["zone_id"];
		$cbol=false;
		$rule=get_root(0,1,$z["zone_id"]);
		$tmp='<tr><td>'.$z["zone_name"].'</td><td align="right"><nobr>'.root(lng('Entrance'),$ids.':view',0,$rule->view,check_zone($z["zone_id"],'view')).$sep.root(lng('Statistics'),$ids.':rules',0,$rule->rules,check_zone($z["zone_id"],'rules')).$sep.root(lng('Control'),$ids.':edit',0,$rule->edit,check_zone($z["zone_id"],'edit')).'</nobr></td></tr>';	
		if(check_zone($z["zone_id"],'view')){
			if($cbol) $tvis0='';
			$t2.=$tmp;
		}
	}
	$t2.='</table></td></tr>';
	$t2=str_replace('TVIS0',$tvis0,$t2);
	$content.=$t2;
}

$rule=get_root(1,0,0);
if(empty($group_module)){
	$content.='<tr><td colspan="2"><b>'.lng('Modules').'</b></td></tr>';
	$content.='<tr><td>'.lng('All modules').'</td><td align="right"><nobr>'.root(lng('Adding'),'1:0:add',0,$rule->add,check_mod(0,'add')).$sep.root(lng('Deleting'),'1:0:del',0,$rule->del,check_mod(0,'del')).$sep.root(lng('View'),'1:0:view',0,$rule->view,check_mod(0,'view')).$sep.root(lng('Control'),'1:0:edit',0,$rule->edit,check_mod(0,'edit')).'</nobr></td></tr>';
	$rule=get_root(3,0,0);
	$content.='<tr><td>'.lng('All tables').'</td><td align="right"><nobr>'.root(lng('View'),'3:0:0:view',0,$rule->view,check_tbl(0,'view')).'</nobr></td></tr>';
	$rule=get_root(2,0,0);
	$content.='<tr><td>'.lng('All sections').'</td><td align="right"><nobr>'.root(lng('Adding'),'2:0:0:add',0,$rule->add,check_ex(0,'add')).$sep.root(lng('Deleting'),'2:0:0:del',0,$rule->del,check_ex(0,'del')).$sep.root(lng('View'),'2:0:0:view',0,$rule->view,check_ex(0,'view')).$sep.root(lng('Control'),'2:0:0:edit',0,$rule->edit,check_ex(0,'edit')).'</nobr></td></tr>';
	$ids2='4c:';
	$rule2=get_root(4,0,0);
	$content.='<tr><td>'.lng('All objects').'</td><td align="right"><nobr>'.root(lng('View'),$ids2.':view',2,$rule2->view,check_row(0,0,0,'view',0,Array(),0,1)).$sep.root(lng('Adding'),$ids2.':add',0,$rule2->add,check_row(0,0,0,'add',0,Array(),0,1)).$sep.root(lng('Modify'),$ids2.':edit',2,$rule2->edit,check_row(0,0,0,'edit',0,Array(),0,1)).$sep.root(lng('Deleting'),$ids2.':del',2,$rule2->del,check_row(0,0,0,'del',0,Array(),0,1)).'</nobr></td></tr>';
	
	$ms=getall($db,"SELECT module_id, module_name FROM main_module ORDER BY module_name",1,"main_module");
	$cls=getall($db,"SELECT col_id, col_name, col_table FROM main_col ORDER BY col_name",1,"main_col");
	$cl=Array();
	if(!empty($cls)) foreach($cls AS $c) $cl[$c["col_table"]][$c["col_id"]]=$c;
	$tst=getall($db,"SELECT table_id, table_module, table_name FROM main_table ORDER BY table_name",1,"main_table");
	$ts=Array();
	if(!empty($tst)) foreach($tst AS $t) $ts[$t["table_module"]][$t["table_id"]]=$t;
	$ext=getall($db,"SELECT ex_name, ex_module, ex_id FROM ex_module ORDER BY ex_name",1,"ex_module");
	$ex=Array();
	if(!empty($ext)) foreach($ext AS $e) $ex[$e["ex_module"]][$e["ex_id"]]=$e;

	$tmp=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 ORDER BY group_name",1,"main_auth");
	if(!empty($tmp)) foreach($tmp AS $t) $grp[$t["group_owner"]][$t["auth_id"]]=$t;	
} else {
	$ms=getall($db,"SELECT * FROM main_module WHERE module_id=$group_module ORDER BY module_name",1,"main_module");
	$cls=getall($db,"SELECT * FROM main_col WHERE col_module=$group_module ORDER BY col_name",1,"main_col");
	$cl=Array();
	if(!empty($cls)) foreach($cls AS $c) $cl[$c["col_table"]][$c["col_id"]]=$c;
	$tst=getall($db,"SELECT * FROM main_table WHERE table_module=$group_module ORDER BY table_name",1,"main_table");
	$ts=Array();
	if(!empty($tst)) foreach($tst AS $t) $ts[$t["table_module"]][$t["table_id"]]=$t;
	$ext=getall($db,"SELECT * FROM ex_module WHERE ex_module=$group_module ORDER BY ex_name",1,"ex_module");
	$ex=Array();
	if(!empty($ext)) foreach($ext AS $e) $ex[$e["ex_module"]][$e["ex_id"]]=$e;
	
	$tmp=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_module=$group_module ORDER BY group_name",1,"main_auth");
	if(!empty($tmp)) foreach($tmp AS $t) $grp[$t["group_owner"]][$t["auth_id"]]=$t;	
}

if(!empty($ms)) foreach($ms AS $m){
	$ids='1:'.$m["module_id"];
	$rule=get_root(1,1,$m["module_id"]);
	$t1='<tr><td><span class="link" OnClick="showhide(\'tri'.$m["module_id"].'\');"><b>'.$m["module_name"].'</b></span></td><td align="right"><nobr>'.root(lng('View'),$ids.':view',0,$rule->view,check_mod($m["module_id"],'view')).$sep.root(lng('Control'),$ids.':edit',0,$rule->edit,check_mod($m["module_id"],'edit')).'</nobr></td></tr>';	
 	$t1.='<tr id="tri'.$m["module_id"].'" style="display: TVIS1;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
	$ids='3:'.$m["module_id"].':0';
	$rule=get_root(3,3,$m["module_id"]);
	$ids2='4d:'.$m["module_id"];
	$rule2=get_root(4,5,$m["module_id"]);
	$cbol=false;
	$t2='<tr><td>&nbsp;&nbsp;<span class="link" OnClick="showhide(\'tr'.$m["module_id"].'\');">'.lng('All module tables').' «'.$m["module_name"].'»</span></td><td align="right"><nobr>'.root(lng('View'),$ids.':view',0,$rule->view,check_tbl(0,'view',$m["module_id"])).$sep.lng('Objects (all tables)').':&nbsp; '.root(lng('View'),$ids2.':view',2,$rule2->view,check_row(0,0,0,'view',0,Array(),$m["module_id"],1)).$sep.root(lng('Adding'),$ids2.':add',0,$rule2->add,check_row(0,0,0,'add',0,Array(),$m["module_id"],1)).$sep.root(lng('Modify'),$ids2.':edit',2,$rule2->edit,check_row(0,0,0,'edit',0,Array(),$m["module_id"],1)).$sep.root(lng('Deleting'),$ids2.':del',2,$rule2->del,check_row(0,0,0,'del',0,Array(),$m["module_id"],1)).'</nobr></td></tr>';
	$tvis1='none';
	$tvis2='none';
	if($cbol) $tvis1='';
 	$t2.='<tr id="tr'.$m["module_id"].'" style="display: TVIS2;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
 	$t3='';
	if(!empty($ts[$m["module_id"]])) foreach($ts[$m["module_id"]] AS $t){
		$ids='3:'.$m["module_id"].':'.$t["table_id"];
		$rule=get_root(3,1,$t["table_id"]);
 		$ids2='4a:'.$t["table_id"];
		$rule2=get_root(4,3,$t["table_id"]);
		$t4='<tr><td>&nbsp;&nbsp;';
		if(!empty($cl[$t["table_id"]])) $t4.='<span class="link" OnClick="showhide(\'tr7b'.$t["table_id"].'\');">';
		$t4.=' - '.$t["table_name"];
		if(!empty($cl[$t["table_id"]])) $t4.='</span>';
		$cbol=false;
		$t4.='</td><td align="right" align="right"><nobr>'.root(lng('View'),$ids.':view',0,$rule->view,check_tbl($t["table_id"],'view')).$sep.lng('Objects').':&nbsp; '.root(lng('View'),$ids2.':view',2,$rule2->view,check_row(0,$t["table_id"],0,'view',0,Array(),0,1)).$sep.root(lng('Adding'),$ids2.':add',/*0*/5,$rule2->add,check_row(0,$t["table_id"],0,'add',0,Array(),0,1)).$sep.root(lng('Modify'),$ids2.':edit',2,$rule2->edit,check_row(0,$t["table_id"],0,'edit',0,Array(),0,1)).$sep.root(lng('Deleting'),$ids2.':del',2,$rule2->del,check_row(0,$t["table_id"],0,'del',0,Array(),0,1)).'</nobr></td></tr>';
		if($cbol) $tvis1='';
		if($cbol) $tvis2='';
		if(!empty($cl[$t["table_id"]]) || !empty($stables[$t["table_id"]])){
			$tvis3='none';
		 	$t4.='<tr id="tr7b'.$t["table_id"].'" style="display: TVIS3;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
		 	$t5='';
			if(!empty($cl[$t["table_id"]])) foreach($cl[$t["table_id"]] AS $c){
				$ids='7:'.$c["col_id"];
				$rule=get_root(7,1,$c["col_id"]);
				$cbol=false;
				$tt='<tr><td> &nbsp;&nbsp;&nbsp;&nbsp;- '.lng('Field').' «'.$c["col_name"].'»</td><td align="right" align="right"><nobr>'.root(lng('View'),$ids.':view',0,$rule->view,check_col($c["col_id"],'view')).$sep.root(lng('Editing/Adding'),$ids.':edit',0,$rule->edit,check_col($c["col_id"],'edit')).'</nobr></td></tr>';
				if(check_col($c["col_id"],'view')) $t5.=$tt;
				if($cbol){
					$tvis1='';$tvis2='';$tvis3='';
				}
			}
			if(!empty($stables[$t["table_id"]])) $t5.=echo_subtables($t["table_id"],Array($t["table_id"]));
			$t5.='</table></td></tr>';
			$t4=str_replace('TVIS3',$tvis3,$t4);
			$t4.=$t5;			
		}
		if(check_tbl($t["table_id"],'view')) $t3.=$t4;
	}
	$t2=str_replace('TVIS2',$tvis2,$t2);
	$t2.=$t3.'</table></td></tr>';
	
	$ids='2:'.$m["module_id"].':0';
	$rule=get_root(2,3,$m["module_id"]);
	if($cbol) $tvis1='';
	if(!empty($ex[$m["module_id"]])) $t3= '<tr><td>&nbsp;&nbsp;<span class="link" OnClick="showhide(\'er'.$m["module_id"].'\');">'.lng('All module sections').' «'.$m["module_name"].'»</span></td><td align="right"><nobr>'.root(lng('Adding'),$ids.':add',0,$rule->add,check_ex(0,'add',$m["module_id"])).$sep.root(lng('Deleting'),$ids.':del',0,$rule->del,check_ex(0,'del',$m["module_id"])).$sep.root(lng('View'),$ids.':view',0,$rule->view,check_ex(0,'view',$m["module_id"])).$sep.root(lng('Control'),$ids.':edit',0,$rule->edit,check_ex(0,'edit',$m["module_id"])).'</nobr></td></tr>';
	else  $t3= '<tr><td>&nbsp;&nbsp;'.lng('All module sections').' «'.$m["module_name"].'»</td><td align="right"><nobr>'.root(lng('Adding'),$ids.':add',0,$rule->add,check_ex(0,'add',$m["module_id"])).$sep.root(lng('Deleting'),$ids.':del',0,$rule->del,check_ex(0,'del',$m["module_id"])).$sep.root(lng('View'),$ids.':view',0,$rule->view,check_ex(0,'view',$m["module_id"])).$sep.root(lng('Control'),$ids.':edit',0,$rule->edit,check_ex(0,'edit',$m["module_id"])).'</nobr></td></tr>';
	$tvis2='none';
 	$t3.='<tr id="er'.$m["module_id"].'" style="display: TVIS2;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
 	$t4='';
	if(!empty($ex[$m["module_id"]])) foreach($ex[$m["module_id"]] AS $e){
		$ids='2:'.$m["module_id"].':'.$e["ex_id"];
		$rule=get_root(2,1,$e["ex_id"]);
 		$ids2='4b:'.$e["ex_id"];
		$rule2=get_root(4,4,$e["ex_id"]);
		$cbol=false;
		$tt='<tr><td>&nbsp;&nbsp; - '.$e["ex_name"].'</td><td align="right"><nobr>'.root(lng('View'),$ids.':view',0,$rule->view,check_ex($e["ex_id"],'view')).$sep.root(lng('Control'),$ids.':edit',0,$rule->edit,check_ex($e["ex_id"],'edit')).$sep.lng('Objects').':&nbsp; '.root(lng('View'),$ids2.':view',2,$rule2->view,check_row(0,0,$e["ex_id"],'view',0,Array(),$m["module_id"],1)).$sep.root(lng('Adding'),$ids2.':add',0,$rule2->add,check_row(0,0,$e["ex_id"],'add',0,Array(),$m["module_id"],1)).$sep.root(lng('Modify'),$ids2.':edit',2,$rule2->edit,check_row(0,0,$e["ex_id"],'edit',0,Array(),$m["module_id"],1)).$sep.root(lng('Deleting'),$ids2.':del',2,$rule2->del,check_row(0,0,$e["ex_id"],'del',0,Array(),$m["module_id"],1)).'</nobr></td></tr>';
		if(check_ex($e["ex_id"],'view')) $t4.=$tt;
		if($cbol){
			$tvis1='';
			$tvis2='';
		}
	}	
	$t4.='</table></td></tr>';	
	
	$r='6a:'.$m["module_id"];
	$rule2=get_root(6,3,$m["module_id"]);
	if($cbol) $tvis1='';
	$tmp=echo_root(0,$m["module_id"]);
	$t5='<tr><td>&nbsp;&nbsp;';
	if(!empty($tmp)) $t5.='<span class="link" onclick="showhide(\'groupes'.$m["module_id"].'\');">';
	$t5.=lng('All module users').' «'.$m["module_name"].'»';
	if(!empty($tmp)) $t5.='</span>';
	$t5.='</td><td align="right"><nobr>'.root(lng('View'),'6:'.$r.':view',2,$rule2->view,check_user(0,'view',0,0,1,$m["module_id"])).$sep.root(lng('Permissions'),'6:'.$r.':rules',2,$rule2->rules,check_user(0,'rules',0,0,1,$m["module_id"])).$sep.root(lng('Modify'),'6:'.$r.':edit',2,$rule2->edit,check_user(0,'edit',0,0,1,$m["module_id"])).$sep.root(lng('Deleting'),'6:'.$r.':del',2,$rule2->del,check_user(0,'del',0,0,1,$m["module_id"])).$sep.root(lng('Register'),'6:'.$r.':reg',0,$rule2->reg,check_user(0,'reg',0,0,1,$m["module_id"])).'</nobr></td></tr>';
	$t5.='<tr id="groupes'.$m["module_id"].'" style="display: TVIS1;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
	if($GLOBALS["back"]) $t5=str_replace('TVIS1','',$t5); else $t5=str_replace('TVIS1','none',$t5);
	$t5.=$tmp;
	$t5.='</table></td></tr>';
		
	$r='8a:'.$m["module_id"];
	$rule2=get_root(8,3,$m["module_id"]);
	if($cbol) $tvis1='';
	$tmp=echo_root2(0,$m["module_id"]);
	$t6='<tr><td>&nbsp;&nbsp;';
	if(!empty($tmp)) $t6.='<span class="link" onclick="showhide(\'groupesx'.$m["module_id"].'\');">';
	$t6.=lng('All module groups').' «'.$m["module_name"].'»';
	if(!empty($tmp)) $t6.='</span>';
	$t6.='</td><td align="right"><nobr>'.root(lng('View'),'8:'.$r.':view',0,$rule2->view,check_group(0,'view',1,$m["module_id"])).$sep.root(lng('Add.'),'8:'.$r.':add',0,$rule2->add,check_group(0,'add',1,$m["module_id"])).$sep.root(lng('Edt.'),'8:'.$r.':edit',2,$rule2->edit,check_group(0,'edit',1,$m["module_id"])).$sep.root(lng('Deleting'),'8:'.$r.':del',2,$rule2->del,check_group(0,'del',1,$m["module_id"])).$sep.root(lng('Permissions'),'8:'.$r.':rules',2,$rule2->rules,check_group(0,'rules',1,$m["module_id"])).$sep.root(lng('Inviting'),'8:'.$r.':invite',2,$rule2->invite,check_group(0,'invite',1,$m["module_id"])).$sep.root(lng('Kicking'),'8:'.$r.':leave',2,$rule2->leave,check_group(0,'leave',1,$m["module_id"])).'</nobr></td></tr>';
	$t6.='<tr id="groupesx'.$m["module_id"].'" style="display: TVIS1;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
	if($GLOBALS["back"]) $t6=str_replace('TVIS1','',$t6); else $t6=str_replace('TVIS1','none',$t6);
	$t6.=$tmp;
	$t6.= '</table></td></tr>';

	$t1=str_replace('TVIS1',$tvis1,$t1);
	$t3=str_replace('TVIS2',$tvis2,$t3);
	if(check_mod($m["module_id"],'view')){
		$content.=$t1.$t2.$t3.$t4.$t5.$t6;
		$content.='</table></td></tr>';
	}
}
//для строк x:y:name - x = 4, y=0, значит для всех.
// x:y:z:a:name - x=4, y = таблица или 0, z = экземпляр или 0, a = 0, значит для всех строк экземпляра Z или таблицы Y
 
//для строк x:y:name - x = 4, y=0, значит для всех.
// x:y:z:name - x=4, y = таблица или 0, z = экземпляр или 0 - для всех строк экземпляра Z или таблицы Y

$dir='files';
if(!$user->super) $dir=$user->folder;
$fldrs=scan_dir(DOCUMENT_ROOT.'/'.$dir,Array(),1,1,0,0,1,30,1);
$fldrs_used=Array();

function echo_dir($dir,$step=0){
	stf('echo_dir');
	global $fldrs,$fldrs_used;
	if(filename($dir)=='editor' || filename($dir)=='js') return Array();
	global $sep,$cbol;
	if(!empty($fldrs[$dir]))$rs=$fldrs[$dir]; else $rs=Array();	
	$res='';
	$back=false;
	$back2=false;

	foreach($rs AS $r){
		if(!empty($fldrs[$r]))$tmp=$fldrs[$r]; else $tmp=Array();
		if($step>1) $tmp='';//3 - макс. кол-во вложений для обзора
		$br=$r;
		$r=substr($r,strlen(DOCUMENT_ROOT)+1,strlen($r)-strlen(DOCUMENT_ROOT)-1);
		$fldrs_used[$r]=$r;
		$br2=str_replace('/','-',$r);
		$name=filename($r);
		if($name=='js') continue;
		if($name=='editor') continue;
		if(!empty($tmp) && $step<1) $name='<span class="link" onclick="showhide(\'f-'.$br2.'\');">'.$name.'</span>';
		$name=dblchar('&nbsp;&nbsp;',$step).' - '.$name;
		$rule=get_root2(5,1,$r);
		$cbol=false;
		$res2='<tr><td>'.$name.'</td><td align="right"><nobr>'.root(lng('View'),'5:'.$r.':view',0,$rule->view,check_folder($r,'view')).$sep.lng('Files').': '.root(lng('Uploading/Subfolder creation'),'5:'.$r.':edit',0,$rule->edit,check_folder($r,'edit')).$sep.root(lng('Deleting'),'5:'.$r.':del',0,$rule->del,check_folder($r,'del'))./*$sep.root(lng('Modify'),'5:'.$r.':edit').*/'</nobr></td></tr>';
		if($cbol){
			$back=true;
		}
		if(check_folder($r,'view')) $back2=true;
		if(!empty($tmp) && $step<1){
			$t1='<tr id="f-'.$br2.'" style="display: TVIS1;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
			$tmp=echo_dir(/*'/'.*/$br,$step+1);
			if($GLOBALS["back2"]){
				$back2=true;
			}
			if($GLOBALS["back"]){
				$t1=str_replace('TVIS1','',$t1);
				$back=true;
			} else $t1=str_replace('TVIS1','none',$t1);
			$res2.=$t1.$tmp.'</table></td></tr>';
		}
		if(check_folder($r,'view') || isset($GLOBALS["back2"]) && $GLOBALS["back2"]) $res.=$res2;
		$GLOBALS["back2"]=false;
	}
	$GLOBALS["back"]=$back;
	$GLOBALS["back2"]=$back2;
	etf('echo_dir');
	return $res;
}
function echo_dir2($dir,$step=0){
	global $fselect, $fldrs, $content;
	stf('echo_dir2');
	if(filename($dir)=='editor' || filename($dir)=='js') return Array();
	global $sep;
	//$rs=scan_dir($dir,Array(),0,1);
	if(!empty($fldrs[$dir]))$rs=$fldrs[$dir]; else $rs=Array();

	$r=substr($dir,strlen(DOCUMENT_ROOT)+1,strlen($dir)-strlen(DOCUMENT_ROOT)-1);
	if(check_folder($r,'view')) foreach($rs AS $r){
		//$tmp=scan_dir($r,Array(),0,1);
		if(!empty($fldrs[$r]))$tmp=$fldrs[$r]; else $tmp=Array();
		if($step>1) $tmp='';//3 - макс. кол-во вложений для обзора
		$br=$r;
		$r=substr($r,strlen(DOCUMENT_ROOT)+1,strlen($r)-strlen(DOCUMENT_ROOT)-1);
		if(check_folder($r,'view')){
			$br2=str_replace('/','-',$r);
			$name=filename($r);
			if($name=='js') continue;
			if($name=='editor') continue;
			if($step!=0) $name=dblchar('&nbsp;&nbsp;',$step).' - '.$name;
			$rule=get_root2(5,1,$r);
			$add='';
			if($GLOBALS["auth_folder2"]==$r){ $add=' selected'; $fselect=true;}
			$content.='<option value="'.$r.'"'.$add.'>'.$name.'</option>';
			if(!empty($tmp) && $step<1){
				echo_dir2($br,$step+1);
			}
		}
	}
	etf('echo_dir2');
}
if(empty($fmod) || $id!=0){
	$content.='<tr><td colspan="2"><b>'.lng('Folders').'</b></td></tr>';
	$r='0';
	$rule=get_root2(5,0,'0');
	$content.='<tr><td><span class="link" onclick="showhide(\'files\');">'.lng('All').'</span></td><td align="right"><nobr>'.root(lng('View'),'5:'.$r.':view',0,$rule->view,check_folder($r,'view')).$sep.'Файлы: '.root(lng('Uploading/Subfolder creation'),'5:'.$r.':edit',0,$rule->edit,check_folder($r,'edit')).$sep.root(lng('Deleting'),'5:'.$r.':del',0,$rule->del,check_folder($r,'del'))/*.$sep.root(lng('Modify'),'5:'.$r.':edit')*/.'</nobr></td></tr>';
	$t1='<tr id="files" style="display: TVIS1;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
	$tmp=echo_dir(DOCUMENT_ROOT./*'/files'.*/'/'.$dir);
	if($GLOBALS["back"]) $t1=str_replace('TVIS1','',$t1); else $t1=str_replace('TVIS1','none',$t1);
	$content.=$t1.$tmp;
	$content.='</table></td></tr>';
	perm_folder();
	getrow($db,"SELECT * FROM auth_perm WHERE perm_auth=$id AND perm_type=5 AND perm_target=6");
	if(!empty($db->Record)) $GLOBALS["auth_folder2"]=$db->Record["perm_folder"]; else $GLOBALS["auth_folder2"]='';
}

// Отображение тех папок, доступ к которым обозначен, но которые пользователь видеть не может
$fldrs_rules=getall3($db,"SELECT * FROM auth_perm WHERE perm_type=5 AND perm_target=1 AND perm_auth=$id",'perm_folder');
$content.='<div style="display: none;">'; //прячем от любознательных ручек
if(!empty($fldrs_rules)) foreach($fldrs_rules AS $fld) if(!isset($fldrs_used[$fld])){
	$r=$fld;
	$rule=get_root2(5,1,$r);	
	$content.=root(lng('View'),'5:'.$r.':view',0,$rule->view,check_folder($r,'view'));
	$content.=root(lng('Uploading/Subfolder creation'),'5:'.$r.':edit',0,$rule->edit,check_folder($r,'edit'));
	$content.=root(lng('Deleting'),'5:'.$r.':del',0,$rule->del,check_folder($r,'del'));
}
$content.='</div>';

// Определение рабочего каталога
if(($user->super || $user->folder!='') && (empty($fmod) || $id!=0)){
	$content.='<tr><td>'.lng('Working folder').':</td><td align="right">';
	$content.='<select class="button" name="auth_folder2">';
	$sel1='';
	if($GLOBALS["auth_folder2"]=="") $sel1=' selected';
	$content.='<option value=""'.$sel1.'>'.lng('No (defined by chields or parents)').'</option>';
	global $fselect;
	$fselect=false;
	if($GLOBALS["auth_folder2"]==$user->folder && $user->folder!=''){$fselect=true; $add=' selected';} else $add='';	

	$dir='files';
	if($user->folder!='' && !$user->super) $content.='<option value="'.$user->folder.'"'.$add.'>'.lng('Root folder').' - '.$user->folder.'</option>';
	else $content.='<option value="'.$dir.'"'.$add.'>'.lng('Root folder').' - files</option>';
	
	if(!$user->super) $dir=$user->folder;
	echo_dir2(DOCUMENT_ROOT.'/'.$dir);
	if($GLOBALS["auth_folder2"]=="") $fselect=true;
	if(!$fselect){
		$content.='<option value="'.spcode($GLOBALS["auth_folder2"],'auth_folder2').'" selected>('.lng('save old value').') '.$GLOBALS["auth_folder2"].'</option>';
	}
	$content.='</select></td></tr>';
}

function echo_root($owner,$module=0,$step=0){
	global $grp,$sep,$cbol;
	$back=false;
	$res='';
	if(!empty($grp[$owner])) foreach($grp[$owner] AS $g)if($g["group_module"]==$module){
		$name=$g["group_name"];
		if(!empty($grp[$g["auth_id"]])) $name='<span class="link" onclick="showhide(\'grp'.$g["auth_id"].'\');">'.$name.'</span>';
		$name=dblchar('&nbsp;&nbsp;',$step).$name;
		$r=$g["auth_id"];
	
		$ids='6:'.$g["auth_id"];
		$rule=get_root(6,1,$g["auth_id"]);
		
		$cbol=false;
		$t1='<tr><td>&nbsp;&nbsp; - '.$name.'</td><td align="right"><nobr>'.root(lng('View'),$ids.':view',2,$rule->view,check_user($g["auth_id"],'view',0,0,1)).$sep.root(lng('Permissions'),$ids.':rules',2,$rule->rules,check_user($g["auth_id"],'rules',0,0,1)).$sep.root(lng('Modify'),'6:'.$r.':edit',2,$rule->edit,check_user($g["auth_id"],'edit',0,0,1)).$sep.root(lng('Deleting'),$ids.':del',2,$rule->del,check_user($g["auth_id"],'del',0,0,1)).'</nobr></td></tr>';
		if($cbol){
			$back=true;
		}
		if(!empty($grp[$g["auth_id"]])){
			$t1.='<tr id="grp'.$g["auth_id"].'" style="display: TVIS1;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
			$tmp=echo_root($g["auth_id"],$module,$step+1);
			if($GLOBALS["back"]){
				$t1=str_replace('TVIS1','',$t1);
				$back=true;
			} else $t1=str_replace('TVIS1','none',$t1);
			$t1.=$tmp.'</table></td></tr>';
		}
		if(check_user($g["auth_id"],'view')) $res.=$t1;
	}
	$GLOBALS["back"]=$back;
	return $res;
}
if(empty($fmod) && empty($group_module)/* || $id!=0*/){
	$content.='<tr><td colspan="2"><b>'.lng('Users').'</b></td></tr>';
	$r=0;
	$rule2=get_root(6,0,0);
	if(empty($group_module)){
		$t1='<tr><td><span class="link" onclick="showhide(\'groupes\');">'.lng('All').'</span></td><td align="right"><nobr>'.root(lng('View'),'6:'.$r.':view',2,$rule2->view,check_user(0,'view',0,0,1)).$sep.root(lng('Permissions'),'6:'.$r.':rules',2,$rule2->rules,check_user(0,'rules',0,0,1)).$sep.root(lng('Modify'),'6:'.$r.':edit',2,$rule2->edit,check_user(0,'edit',0,0,1)).$sep.root(lng('Deleting'),'6:'.$r.':del',2,$rule2->del,check_user(0,'del',0,0,1)).$sep.root(lng('Register'),'6:'.$r.':reg',0,$rule2->reg,check_user(0,'reg',0,0,1)).'</nobr></td></tr>';
		$t1.='<tr id="groupes" style="display: TVIS1;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
	} else {
		$t1='';
	}
	$tmp=echo_root(0);
	if($GLOBALS["back"]) $t1=str_replace('TVIS1','',$t1); else $t1=str_replace('TVIS1','none',$t1);
	$content.=$t1.$tmp;
	if(empty($group_module)) $content.='</table>';
	$content.='</td></tr>';
}


function echo_root2($owner,$module=0,$step=0){
	global $grp,$sep,$cbol;
	$back=false;
	$res='';
	if(!empty($grp[$owner])) foreach($grp[$owner] AS $g)if($g["group_module"]==$module){
		$name=$g["group_name"];
		if(!empty($grp[$g["auth_id"]])) $name='<span class="link" onclick="showhide(\'grpx'.$g["auth_id"].'\');">'.$name.'</span>';
		$name=dblchar('&nbsp;&nbsp;',$step).$name;
	
		$r=$g["auth_id"];
		$rule2=get_root(8,1,$g["auth_id"]);

		$cbol=false;		
		$t1='<tr><td>&nbsp;&nbsp; - '.$name.'</td><td align="right"><nobr>'.root(lng('View'),'8:'.$r.':view',0,$rule2->view,check_group($g["auth_id"],'view',1)).$sep.root(lng('Permissions'),'8:'.$r.':rules',0,$rule2->rules,check_group($g["auth_id"],'rules',1)).$sep.root(lng('Inviting'),'8:'.$r.':invite',0,$rule2->invite,check_group($g["auth_id"],'invite',1)).$sep.root(lng('Kicking'),'8:'.$r.':leave',0,$rule2->leave,check_group($g["auth_id"],'leave',1)).'</nobr></td></tr>';
		if($cbol){
			$back=true;
		}
		if(!empty($grp[$g["auth_id"]])){
			$t1.='<tr id="grpx'.$g["auth_id"].'" style="display: TVIS1;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
			$tmp=echo_root2($g["auth_id"],$module,$step+1);
			if($GLOBALS["back"]){
				$t1=str_replace('TVIS1','',$t1);
				$back=true;
			} else $t1=str_replace('TVIS1','none',$t1);
			$t1.=$tmp.'</table></td></tr>';
		}
		if(check_group($g["auth_id"],'view')) $res.=$t1;
	}
	$GLOBALS["back"]=$back;
	return $res;
}

if(empty($fmod) && empty($group_module)){
	$content.='<tr><td colspan="2"><b>'.lng('Groups').'</b></td></tr>';
	$r=0;
	
	if(empty($group_module)){
		$rule2=get_root(8,0,0);
		$t1='<tr><td><span class="link" onclick="showhide(\'groupesx\');">'.lng('All').'</span></td><td align="right"><nobr>'.root(lng('View'),'8:'.$r.':view',0,$rule2->view,check_group(0,'view',1)).$sep.root(lng('Add.'),'8:'.$r.':add',0,$rule2->add,check_group(0,'add',1)).$sep.root(lng('Edt.'),'8:'.$r.':edit',2,$rule2->edit,check_group(0,'edit',1)).$sep.root(lng('Deleting'),'8:'.$r.':del',2,$rule2->del,check_group(0,'del',1)).$sep.root(lng('Permissions'),'8:'.$r.':rules',2,$rule2->rules,check_group(0,'rules',1)).$sep.root(lng('Inviting'),'8:'.$r.':invite',2,$rule2->invite,check_group(0,'invite',1)).$sep.root(lng('Kicking'),'8:'.$r.':leave',2,$rule2->leave,check_group(0,'leave',1)).'</nobr></td></tr>';
		$t1.='<tr id="groupesx" style="display: TVIS1;" class="np"><td colspan="2" class="np"><table id="records" cellpadding=0 cellspacing=0>';
	} else {
		$t1='';
	}
	$tmp=echo_root2(0,0);
	if($GLOBALS["back"]) $t1=str_replace('TVIS1','',$t1); else $t1=str_replace('TVIS1','none',$t1);
	$content.=$t1.$tmp;
	if(empty($group_module)) $content.='</table>';
	$content.='</td></tr>';
}


$content.='</table>';

$form_vars['path']='perm';
$form_vars['form_type']='edit';
$form_vars['OnSubmit']='return colct();';
$form_vars['section']['main']['fields'][]=Array('type'=>'hidden', 'name'=>'id', 'value'=>$id);
$form_vars['section']['main']['fields'][]=Array('type'=>'hidden', 'name'=>'action', 'value'=>'edit');
$form_vars['section']['main']['fields'][]=Array('type'=>'hidden', 'name'=>'collect', 'value'=>'','id'=>'collect');
$form_vars['section']['main']['fields'][]=Array('type'=>'hidden', 'name'=>'spec', 'value'=>spcode($cspec,'spec'));
$form_vars['section']['main']['fields'][]=Array('type'=>'static', 'content'=>$content);
$form_vars['section']['main']['fields'][]=Array('type'=>'static', 'content'=>$madd2.get_form_protection_key('perm',1,1));
if(!empty($fmod)) $form_vars['section']['main']['fields'][]=Array('type'=>'hidden', 'name'=>'fmod', 'value'=>$fmod);
if(!empty($fmod)) $form_vars['go_back_url']='perm?id='.$id.'&fmod='.$fmod;
else $form_vars['go_back_url']='perm?id='.$id;

echo shell_tpl_admin('block/form',$form_vars);

//if(!empty($fmod)) echo '<input type="hidden" name="fmod" value="'.$fmod.'">';
//echo '<br><input class="button" type="submit" value="Сохранить"> или <a href="perm?id='.$id.(!empty($fmod)?'&amp;fmod='.$fmod:'').'">вернуться назад</a>';

/*echo '<input type="hidden" name="spec" value="'.spcode($cspec,'spec').'">';
echo $madd2;
echo '<input type="hidden" id="collect" name="collect" value="">';
echo '</form><br>';*/

echo '<script>$(".elem").bind("mousedown", function() { return false;});</script>';
echo '<script>var data=Array(';
$first=true;
foreach($inpa AS $name=>$val){
	if(!$first) echo ',';
	$first=false;
	echo '"'.$name.'","'.$val.'"';
}
echo ');
var y="";
var x=data.length; for(var i=0;i<x;i++)if(i%2==0){ document.write(\'<input type="hidden" id="\'+data[i]+\'" name="\'+data[i]+\'" value="\'+data[i+1]+\'">\'); if(y!="") y=y+"&&"; y=y+data[i];}
document.write(\'<input type="hidden" id="collect2" name="collect2" value="\'+y+\'">\');
</script>';

echo '<div align="right"><img src="'.$GLOBALS["base_root"].'/files/editor/up.gif" align="absmiddle"> - '.lng('is not defined (or follow the rules of the parent)').', <img src="'.$GLOBALS["base_root"].'/files/editor/deny.gif" align="absmiddle"> - '.lng('deny').', <img src="'.$GLOBALS["base_root"].'/files/editor/allow.gif" align="absmiddle"> - '.lng('allow').',';
echo '<br><img src="'.$GLOBALS["base_root"].'/files/editor/user3.gif" align="absmiddle"> '.lng('Allowed for users, who are the owners of the object');
echo '<br><img src="'.$GLOBALS["base_root"].'/files/editor/group.gif" align="absmiddle"> '.lng('Allowed for the current user and all its children');
echo '<br><img src="'.$GLOBALS["base_root"].'/files/editor/allow_all.gif" align="absmiddle"> '.lng('Allowed for all subtables');

?>