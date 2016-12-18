<?php

function text2index($text,$add=0){
	global $db,$load_lemma, $new_lemma;
	$text=strip_tags($text);
	$new_lemma=0;
	//$text=/*htmlspecialchars_decode*/html_entity_decode($text/*,ENT_COMPAT | ENT_HTML401*/);
	$text=html_entity_decode($text,ENT_COMPAT);
	$text=clear_dblspace($text);
	$text=trim($text);
	$text=strtolower5($text);
	//$text=strtolower(trim(clear_dblspace(htmlspecialchars_decode(clear_html($text)))));//или html_entity_decode
	$tl=strlen5($text);//mb_strlen($text,'UTF-8');
	$pos=0;
	$text2='';
	//исключаемые знаки
	$except='^:#+=*&$~{}[]<>|';
	$rus='йцукенгшщзхъфывапролджэячсмитьбю';
	$text=str_replace('ё','е',$text);
	$text=str_replace('«','',$text);
	$text=str_replace('»','',$text);
	$text=str_replace('"','',$text);
	for($i=0;$i<$tl;$i++)if(!empty($text[$i]) && strpos($except,$text[$i])===false){
		if($text[$i]=='?') $text2.='.';
		else if($text[$i]=='!') $text2.='.';
		else if($text[$i]=='…') $text2.='.';
		else if($text[$i]=='(') $text2.=',';
		//else if($text[$i]=='"') $text2.='';
		//else if($text[$i]=='«') $text2.='';
		//else if($text[$i]=='»') $text2.='';
		else if($text[$i]==')') $text2.=',';
		else if($text[$i]==';') $text2.=',';
		else if($text[$i]=="'") $text2.='`';
		else if($text[$i]=='.' && !empty($text[$i+1]) && $i>0 && !empty($text[$i-1]) && strstr($rus,$text[$i-1]) && strstr($rus,$text[$i+1])) $text2.='. ';
		else $text2.=$text[$i];
	}
	$text=explode(' ',$text2);
	$res=Array();
	//не индексируемые предлоги (рус, англ)
	$except=array_flip(explode(' ',' вот так как же что и или а то ли бы в с к за на о об обо у но по не уж из под при the what a and or when where it is in about within because instead for of to at above after before below by since with as among around behind beside but by given into like on per than till until'));
	//недопустимые символы в слове, если слово не является емайлом
	$exchar=array_flip(explode(' ',' - _ = + ~ ` " @ # $ % ^ & * ( ) { } [ ] ? > < / \\ \' — –'));
	//$enders=array_flip(explode(' ',' ать а ивать овать ствовать ествовать изовать ировать изировать ства кать ка яка ичать ничать ба оба ытьба ва авый ощавый ева ив иво ивый ливый овливый еливый чливый чивый ов овой овый овь в тва тво ство овство ество инство тельство лявый ага инг ург уга ыга да ад ада иада арад оид ядь е еть ое ые аж ёж ёжь иза оз и ить ки очки ушки нюшки унюшки еньки ошеньки охоньки ами ками ай атай oв1jo ад1ja ей еа алей ачей ий иa иe овий ствие ние ание ование ение арий ерие орий тие циа ациа изациа ициа нциа енциа ачий ичий н1jo aн1jo ен1jo o ой кой т1jo уй тяй кий ко ак ака чак авка овка ловка анек енек онек ышек ежка ик ика ико евик ник овник еник ейник арник атник льник истика чик щик овщик льщик айка ейка лка лки анка инка онка унки ок онок чонок ушок ерка урка ский вский евский овский oвский еский ческий ический истичесий лезский эзский йский ейский ийский имский нский анский ианский енский инский унский тельский етка отка ютка ук чук ацкий ецкий чка ачка ечка ечко ичкаа очка шка шки ашка ёшка ишка ушка ышка ышко ыка улька усенький ошенький онька онький охонький юка як який няк ль ла лый ля ал аль овалый ёл ель ели тель итель ила ол оль ола ули уля ыль изм онизм им имый ом омый oм ком иком ышком няком уном ишом ым нь на ная ни ний ный ня ан аный аня уган иана овиана aн лан ман ебный обный евный ивный овный овня ень ений ёна ёный мен смен яжный знь езный изна овизна озный иозный ин ина ины иный иня бина овина лина елин нин анин жанин ианин чанин овчанин ичанин инчанин тянин итянин атина чина щина овщина льщина йный ейный нный анный ованный ированный енный ённый овенный ственный менный онный ионный ационный он оня арный орный сня снь отный отня ятня ун уня ичный инична очный шний шный ашний ашный ишний ышня льный льня альный идальный иальный ональный уальный ельный абельный ибельный тельный ительный ильный ынь иян о очко енько ошенько онько охонько но овато ар арь ара атарь еро онер мейстер вора тор атор итор ура тура amypa итура ырь яра са иса анс есса уся ариус ть тый am ата атый оватый иат дцать надцать чата чатый евт ет ета итет ит ита итый овитый нит инит ант ент менты амент емент от оть ота иот астый ист истый ость имость ность нность енность тость ута у y ку еньку оньку ому нуть ануть оту ха ах аха ках их иха оха ух уха ца цо цы ец еца ецо авец овец лец омец нец енец инец ица овица лица ница овница еница атница уница ичница очница ешница льница тельница льце ча чий ач ич евич ович ничий учий ыч ша ший аш аша аши иш айший ейший ошь оша уша оныш ащий ище бище овище лище ущий еющий ы жды ажды ою ую остью мя ее ше учи ши вши л вший ам ас ax ая её ем еми емя ex ею ёт ёте ёх ёшь ие ими ите ишь ию м ми ого оё cm ум умя ут шь'));
	//$enders=array_flip(explode(' ',' мя ее ше учи ши вши л вший ам ас ax ая её ем еми емя ex ею ёт ёте ёх ёшь ие ими ите ишь ию м ми ого оё cm ум умя ут шь'));
	//окончания (рус, англ)
	$enders=array_flip(explode(' ',' !их !ох !ах !ых ы и а я у ю о ая ой ие ое ую ий ай аи ь ия те ого ех ся ить ать ять ють уть ее ею ый е ов ова овы ок аю ем ил ом ым ии ых ого ости ость ает ми ыми ляет лять ляют уют ых яет яется яться ят ит ые овые овая овый овое овым овыми ные ным ная ное ный ного ном ение яя ной ными шний шнее шняя шнее шним шними им ьный ьное ьная ьные ьным ьными ьных енный енной енная енное енным енные енными ных ием ическое еческое ическая еческая ический ического ическая еческий ические еческие ических еческих ическим еческим ующего ующая ующие ующих ующим ующее ию аю ою уем ую ует уете уют ях иях ют ет ью ия иям иями ие ием иеми уемых уемым уемыми уемая уемое уемые ающих ающие ающее ающим ающий ающая яшний яшнее яшняя яшних яшние яшним яются яется яться яются яющие яющее яющая яющий яющих яющим ьной ающую ающий ающая ающее ающие ающии ающим ающих ительная ательная ательное ательный отельное отельный отельная ательных ательным отельных отельным ей оваться овался овалась овались овали ают ать ает йный йное йная йные йным йными йных аный аное аная аные аным аными аных  еный еное еная еные еным еными еных ийский ийских ийские ийская ийское ийским s ed ing'));
	$glas='йуеыаоэяию';
	$soglas='цкнгшщзхфвпрлджчсмтб';
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
			//if($word=='педагогический') echo $en.'<br>';
			if(!empty($enders[$en])/* && strlen5($en)<$i2*/){
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
		//echo $pos.' - '.$word.'<br>';
		$res[$cres]->pos=$pos;
		$res[$cres]->word=$word;
		$res[$cres]->old=$sw;
		//echo $word.'<br>';
		//if($pos==76) echo ord($word[0]).' '.ord($word[1]).' ';
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
		//if(!$GLOBALS["correct_utf"]){
		//	getrow($db,"SHOW VARIABLES LIKE 'character_set%'");
		//	$charset=$db->Record["Value"];
		//	getrow($db,"SHOW VARIABLES LIKE 'collation%'");
		//	$collation=$db->Record["Value"];
		//	$db->query("SET NAMES `utf8` COLLATE `utf8_general_ci`");
		//}
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
 				/*$db->query("INSERT INTO index_word (word_lemma) VALUES ('".$r->word."')",3,"index_word");
 				getrow($db,"SELECT LAST_INSERT_ID() as sid");
 				$res[$var]->lemma=$db->Record["sid"]; 				
 				$res[$var]->new=1;*/
 				//$lem[$r->word]=$res[$var]->lemma;
 				
 				$res[$var]->new=1;
 				$after_link[]=&$res[$var];
 				$after_word[]=$r->word;
 				if(count($after_query[$after_index])>=$after_max) $after_index++;
 				$after_query[$after_index][]="('".$r->word."')"; 				
 							
				//$new_lemma++;
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
		//if(!$GLOBALS["correct_utf"]){
		//	$db->query("SET NAMES `".$charset."` COLLATE `".$collation."`");
		//}
	}	
	return $res;
}

?>