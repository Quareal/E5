<?php

function text2index($text,$add=0){
	global $db,$load_lemma, $new_lemma;
	$text=strip_tags($text);
	$new_lemma=0;
	$text=html_entity_decode($text,ENT_COMPAT);
	$text=clear_dblspace($text);
	$text=trim($text);
	$text=strtolower5($text);
	$tl=strlen5($text);
	$pos=0;
	$text2='';
	//исключаемые знаки
	$except='^:#+=*&$~{}[]<>|';
	$eng='qwertyuiopasdfghjklzxcvbnm';	
	$text=str_replace('«','',$text);
	$text=str_replace('»','',$text);
	$text=str_replace('"','',$text);
	for($i=0;$i<$tl;$i++)if(!empty($text[$i]) && strpos($except,$text[$i])===false){
		if($text[$i]=='?') $text2.='.';
		else if($text[$i]=='!') $text2.='.';
		else if($text[$i]=='…') $text2.='.';
		else if($text[$i]=='(') $text2.=',';
		else if($text[$i]==')') $text2.=',';
		else if($text[$i]==';') $text2.=',';
		else if($text[$i]=="'") $text2.='`';
		else if($text[$i]=='.' && !empty($text[$i+1]) && $i>0 && !empty($text[$i-1]) && strstr($eng,$text[$i-1]) && strstr($eng,$text[$i+1])) $text2.='. ';
		else $text2.=$text[$i];
	}
	$text=explode(' ',$text2);
	$res=Array();
	//не индексируемые предлоги (рус, англ)
	$except=array_flip(explode(' ',' the what a and or when where it is in about within because instead for of to at above after before below by since with as among around behind beside but by given into like on per than till until'));
	//недопустимые символы в слове, если слово не является емайлом
	$exchar=array_flip(explode(' ',' - _ = + ~ ` " @ # $ % ^ & * ( ) { } [ ] ? > < / \\ \' — –'));
	//окончания (рус, англ)
	$enders=array_flip(explode(' ',' s ed ing'));
	$glas='qeyuioa';
	$soglas='wrtpsdfghjklzxcvbnm';
	$ctext=count($text);
	$pos=0;
	$cres=0;
	$lemma=Array();
	if(!empty($text)) for($i=0;$i<$ctext;$i++)if(!empty($text[$i])){		
		$word=$text[$i];
		if(!strpos($word,'.') && strpos($word,'-')){
			$word=explode('-',$word);
			$text[$i-1]=$word[0];
			$text[$i]=$word[1];
			$word=$word[0];			
			$i--;
		}
		$aftpos=0;
		$sw=$word;
		while(!empty($word) && $word[strlen5($word)-1]=='.'){ $aftpos+=3; $word=substr5($word,0,strlen5($word)-1);}
		while(!empty($word) && $word[strlen5($word)-1]==','){ $aftpos+=1; $word=substr5($word,0,strlen5($word)-1);}
		while(!empty($word) && $word[0]=='.'){ $pos=$pos+3; $word=substr5($word,1);}
		while(!empty($word) && $word[0]==','){ $pos=$pos+1; $word=substr5($word,1);}
		if(!empty($except[$word])) continue;
		if($word==' ') continue;
		if($word=='.') continue;
		if($word=='') continue;
		$bool=true;
		for($i2=0;$i2<strlen5($word);$i2++)if(empty($exchar[$word[$i2]])){$bool=false; break;}
		if($bool) continue;
		if(strlen5($word)<=2) $start=2;
		if(strlen5($word)==3) $start=2;
		if(strlen5($word)>=4) $start=3;
		//$sw=$word;
		for($i2=$start;$i2<strlen5($word);$i2++){
			$en=substr5($word,$i2);
			if(!empty($enders[$en])){
				if(strstr($glas,$en[0]) && strstr($glas,$word[$i2-1])){
				}else if(strstr($soglas,$en[0]) && strstr($glas,$word[$i2-1])){
				} else {
					$word=substr5($word,0,$i2);
					break;
				}
			}
			if(!empty($enders['!'.$en]) && strlen5($en)<$i2){
				if(strstr($glas,$en[0]) && strstr($glas,$word[$i2-1])){
				}else if(strstr($soglas,$en[0]) && strstr($glas,$word[$i2-1])){
				} else {
					$word=substr5($word,0,$i2);
					break;
				}
			}
		}
		if(mb_strlen($word,'UTF-8')>20) continue;
		$res[$cres]->pos=$pos;
		$res[$cres]->word=$word;
		$res[$cres]->old=$sw;
		$pos++;
		$pos+=$aftpos;
		if(!empty($load_lemma[$word])){
			$res[$cres]->lemma=$load_lemma[$word];
		} else {
			$lemma[$word]=$cres+1;
			$res[$cres]->lemma=0;
		}
		$cres++;
	}
	$new_lemma=count($lemma); //раньше значение означало только новые слова, но теперь - все слова
	$lemma=implode("','",array_flip($lemma));
	if(!empty($lemma)){
		$query="SELECT * FROM index_word WHERE word_lemma IN ('".$lemma."')";
		$lemma=getall($db,$query,1,"index_word");
		$lem=Array();
		if(!empty($lemma)) foreach($lemma AS $lemm) $lem[$lemm["word_lemma"]]=$lemm["word_id"];
		$after_link=Array();
		$after_word=Array();
		$after_query=Array();
		$after_index=0;
		$after_query[$after_index]=Array();
		$after_max=100;
		foreach($res AS $var=>$r) if(empty($r->lemma)){
			if(isset($lem[$r->word])){
				$res[$var]->lemma=$lem[$r->word];
				if($res[$var]->lemma!=0) $load_lemma[$r->word]=$res[$var]->lemma;
			} else if($add){
 				$res[$var]->new=1;
 				$after_link[]=&$res[$var];
 				$after_word[]=$r->word;
 				if(count($after_query[$after_index])>=$after_max) $after_index++;
 				$after_query[$after_index][]="('".$r->word."')"; 
			} else {
				$res[$var]->lemma=-100;
			}
		}
		$first=true;
		$tq=0;
		if(!empty($after_query)) foreach($after_query AS $aq)if(!empty($aq)){
			$cq=count($aq);
			$aq=implode(',',$aq);
			$db->query("INSERT INTO index_word (word_lemma) VALUES ".$aq,3,"index_word");
			if($first){
				//getrow($db,"SELECT LAST_INSERT_ID() as sid");
				//$iq=$db->Record["sid"];
				$iq=$db->last_insert_id();
				$first=false;
			}
			for($i=0;$i<$cq;$i++){
				$ri=$tq+$i;				
 				$after_link[$ri]->lemma=$iq;
				$load_lemma[$after_word[$ri]]=$after_link[$ri]->lemma;
				$iq++;
			}
			$tq+=$cq;
		}
	}	
	return $res;
}

?>