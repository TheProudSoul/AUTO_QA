#pragma once
 // https://www.bunian.cn/so/key/%E5%B0%8F%E5%AD%A6%E5%85%AD%E5%B9%B4%E7%BA%A7%E5%8F%A4%E8%AF%97%E9%80%89%E6%8B%A9%E9%A2%98
#define HARD_COUNT 20;

const char* hard_questions[] = {
"“少壮不努力，老大徒_____”出自《汉乐府·长歌行》。",
"晏殊的《浣溪沙》中的“无可奈何花落去”的下句是_____。",
"《七步诗》的作者是_____。",
"杜甫的《春夜喜雨》中的“晓看红湿处”的下句是_____。",
"“春色满园关不住，一枝红杏出墙来。”出自叶绍翁的_____。",

"“海内存知己，天涯若比邻。”是_____的诗句。",
"“人生自古谁无死，留取丹心照_____。”是文天祥的诗句。",
"“但使龙城飞将在，不教胡马度_____。”是王昌龄的诗句。",
"“停车坐爱枫林晚，霜叶红于二月花。”中的“坐”的意思是_____。",
"杜牧的《江南春》中的“南朝四百八十寺”的下句是_____。",

"“独在异乡为异客，每逢佳节倍思亲。”是_____的诗句。",
"《天净沙 · 秋思》的作者是元代_____的。",
"“野火烧不尽，春风吹又生。”出自_____。",
"“忽如一夜春风来，千树万树梨花开。”写的是_____。",
"“春蚕到死丝方尽，蜡炬成灰泪始干。”出自_____的《无题》。",

"“无边落木萧萧下，不尽长江天际流。“出自杜甫的_____。",
"“三十功名尘与土，八千里路云和月。”是_____的诗句。",
"龚自珍的《己亥杂诗》”落红不是无情物，化着春泥更护花。“中的“红”指的是_____。",
"“海上生明月。_____共此时”是张九龄的诗句。",
"“莫愁前路无知己，天下谁人不识君?”出自_____的 《别董大》。"
};

struct
{
	unsigned int choise_number;
	TCHAR* choise[10];
} hard_choises[] = {
	3, {"悲伤","伤悲","忧伤"},
	3, {"似曾相识鸟归来","似曾相识雁归来","似曾相识燕归来"},
	3, {"曹操","曹丕","曹植"},
	3, {"花重绵阳城","花重锦州城","花重锦官城"},
	3, {"《游园不植》","《春望》","《春夜喜雨》"},

	3, {"王勃","李白","王维"},
	3, {"汉青","汗青","汗清"},
	3, {"阴山","边关","燕山"},
	3, {"因为","坐下","座位"},
	3, {"多少楼台烟波中","多少楼台风雨中","多少楼台烟雨中"},

	3, {"王维","王之涣","王勃"},
	3, {"张养浩","马致远","元好问"},
	3, {"白居易《赋得古原草送别》","王昌龄《出塞》","杜牧《江南春》"},
	3, {"春色","梨花","雪景"},
	3, {"李贺","李清照","李商隐"},

	3, {"《茅屋为秋风所破》","《登高》","《蜀相》"},
	3, {"岳飞","辛弃疾","陆游"},
	3, {"红色","红花","树叶"},
	3, {"天地","天下","天涯"},
	3, {"高适","王昌龄","王勃"}
};


const short hard_answers[] = {
	1, 2, 2, 2, 0,
	0, 1, 0, 0, 2,
	0, 1, 0, 0, 2,
	1, 0, 2, 2, 0
};