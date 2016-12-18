<?php

function latinica($str){
	$str=clear_dblspace($str);
	$str=trim($str);
	$str=utf8_win(strtolower5($str));
	$res='';
	$safe2='0123456789qwertyuiopasdfghjklzxcvbnm-_';
	$con=explode('.',utf8_win(' .й.ц.у.к.е.н.г.ш.щ.з.х.ъ.ф.ы.в.а.п.р.о.л.д.ж.э.я.ч.с.м.и.т.ь.б.ю.ё'));
	$con2=explode('.','-.i.c.u.k.e.n.g.sh.sh.z.h..f.i.v.a.p.r.o.l.d.j.e.ya.ch.s.m.i.t..b.y.yo');
	for($i=0;$i<count($con);$i++) $con3[$con[$i]]=$con2[$i];
	for($i=0;$i<strlen5($safe2);$i++) $safe[$safe2[$i]]=1;
	$br='';
	for($i=0;$i<strlen5($str);$i++){
		$r='';
		$t=$str[$i];
		if(isset($con3[$t])) $r=$con3[$t];
		if(!empty($safe[$t])) $r=$t;
		if(!empty($r) && $r=='-' && $br=='-'){
		} else {
			$res.=$r;
			if(!empty($r)) $br=$r;
		}
	}
	$res=win_utf8($res);
	return $res;
}

function get_num($number,$step=0){
	$spacer=' ';

	$number="$number";	
	if(empty($number) && empty($step)) return 'ноль';

	$third=explode(' ','сто двести триста четыреста пятьсот шестьсот семьсот восемьсот девятьсот');
	$double=explode(' ','десять двадцать тридцать сорок пятьдесят шестьдесят семьдесят восемьдесят девяносто');
	
	//для <4 значных чисел, миллионов, миллиардов и т.д.
	$first=explode(' ','один два три четыре пять шесть семь восемь девять десять одиннадцать двенадцать тринадцать четырнадцать пятнадцать шестнадцать семнадцать восемнадцать девятнадцать');
	//для тысяч
	$first_2=explode(' ','одна две три четыре пять шесть семь восемь девять десять одиннадцать двенадцать тринадцать четырнадцать пятнадцать шестнадцать семнадцать восемнадцать девятнадцать');
	
	$raign=explode(' ','тысяча миллион миллиард триллион квадриллион квинтиллион');
	$raign_2=explode(' ','тысячи миллиона миллиарда триллиона квадриллиона квинтиллиона');
	$raign_3=explode(' ','тысяч миллионов миллиардов триллионов квадриллионов квинтиллионов');
	
	if(strlen5($number)>3){
		$big_number=substr($number,0,strlen5($number)-3);
		$number=substr($number,strlen($number)-3);
		$result=get_num($big_number,$step+1).$spacer;
	} else $result='';
	
	$o_number=$number;
	if(strlen($number)>2){
		$x=$number[0];
		$number=substr($number,1);
		if($x!=0) $result.=$third[$x-1].$spacer;
	}
	if($number>19){
		$x=$number[0];
		$number=substr($number,1);
		if($x!=0) $result.=$double[$x-1].$spacer;
		
		$x=$number[0];
		if($x!=0){
			if($step!=1) $result.=$first[$x-1].$spacer;
			else $result.=$first_2[$x-1].$spacer;
		}
	} else {
		$x=$number;
		if($x!=0){
			if($step!=1) $result.=$first[$x-1].$spacer;
			else $result.=$first_2[$x-1].$spacer;
		}
	}
	
	if($step>0 && ($number!=0 || empty($big_number))){
		$result.=$spacer.get_str_num($o_number,$raign[$step-1],$raign_2[$step-1],$raign_3[$step-1]);
	}
	
	return trim($result);
}

function is_rus($s){
	$rus='йцукенгшщзхфыъвапролджэячсмитьбю';
	for($i=0;$i<strlen5($rus);$i++) if(strpos5($s,$rus[$i])>0) return true;
	return false;
}

function format_o($name){
	$o='о';
	$glas2='йуеыаоэяию';
	for($i=0;$i<strlen5($glas2);$i++) $glas[$glas2[$i]]=1;
	if(!empty($glas[strtolower5($name[0])])) $o='об';
	return $o.' '.$name;
}

?>