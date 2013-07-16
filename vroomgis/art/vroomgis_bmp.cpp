/* Autogenerated by png2wx.pl on Fri Jul  2 14:28:45 2010 */
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#	include <wx/wx.h>
#endif
#include <wx/mstream.h>
#include "vroomgis.h"

wxBitmap *_img_vroomgis;
wxBitmap *_img_vroomgis_sml;

wxBitmap *_img_cursor_editing;
wxBitmap *_img_cursor_zoomin;
wxBitmap *_img_cursor_zoomout;

wxBitmap *_img_tree_folder_on;
wxBitmap *_img_tree_folder_off;

void vroomgis_initialize_images(void)
{
	{
		wxMemoryInputStream sm("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000\020\000\000\000\020\002\003\000\000\000b\235\027\362\000\000\000\011PLTE\000\000\000\377\377\377\000\000\000s\306\203q\000\000\000\001tRNS\000@\346\330f\000\000\000$IDAT\b\327c``t````\233\200\235\020\r\233\032\302\220\265j\325J\b\001\346\342T\0142\n\000$\344\r\325+\360D\321\000\000\000\000IEND\256B`\202", 127);
		_img_cursor_editing = new wxBitmap(wxImage(sm));
	}
	{
		wxMemoryInputStream sm("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000\020\000\000\000\020\002\003\000\000\000b\235\027\362\000\000\000\011PLTE\000\000\000\000\000\000\377\377\377\203\335\317\322\000\000\000\001tRNS\000@\346\330f\000\000\000>IDAT\b\327c`\be```]\025\300\300 \265j\011\220\230\006$\262\246\255d`\310\014\315\204\022`.X\002\254\204uU\bPK\250(\003H\243\003\220`\014\0001A\202\014\"\000\277\251\016\330Hl\214\213\000\000\000\000IEND\256B`\202", 153);
		_img_cursor_zoomin = new wxBitmap(wxImage(sm));
	}
	{
		wxMemoryInputStream sm("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000\020\000\000\000\020\002\003\000\000\000b\235\027\362\000\000\000\011PLTE\000\000\000\000\000\000\377\377\377\203\335\317\322\000\000\000\001tRNS\000@\346\330f\000\000\000:IDAT\b\327c`\be```]\025\300\300 \265j\011\224\310Z\265\222\201!34\023J\200\271\bY\326U!@-\241\242\014 \215\016@\2021\000\304\004\0112\210\000\000\317q\017(\"J\206\257\000\000\000\000IEND\256B`\202", 149);
		_img_cursor_zoomout = new wxBitmap(wxImage(sm));
	}
	{
		wxMemoryInputStream sm("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000\020\000\000\000\020\b\004\000\000\000\265\3727\352\000\000\000\004gAMA\000\001\206\2401\350\226_\000\000\000\347IDAT(\317}\320=2DA\024\305\361_\323S\252\204\004\366@$\223\242\254\347\355b\362\267\002\271\304\n\246&\232\\\"\262\000\201@\011\014\343\275\356+x=\212\242\234\350~\374\373\334\352\223\354tES\177\342A\365\325\203.^\3435\326\261\216ut\341\302\276,\333\225\246}\346E\022`\316B\353\266n\231\001!\011O\346\355ap\337\237[)\231\215\265k?\225\320-\373s\253\314\350\332\274\rc{zrY\366g\231\r\236\005\252PT\025\217\216\340*3H6vT!\214\r\034\rp\230\031\204\2424\240\252\255\032 e>L\020\025\245A\245\345\225\031M\320d]\277N\214[\240\230\262\230\026P\244m\375\236\034w\367\337\377\037\337\322\350o\334&3\247.\035\370\2557w\026\011\311\236\331\037@\361aL\341\177}\002H\374s\322]\002!\026\000\000\000\000IEND\256B`\202", 304);
		_img_tree_folder_off = new wxBitmap(wxImage(sm));
	}
	{
		wxMemoryInputStream sm("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000\020\000\000\000\020\b\003\000\000\000(-\017S\000\000\000\004gAMA\000\001\206\2401\350\226_\000\000\000\347PLTE\000\000\000\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\233\203\021\374\370\346\367\354\267\365\350\247\365\347\245\363\342\223\357\331q\361\336\201\357\331m\366\352\256\366\351\253\365\347\242\364\345\234\363\343\225\363\342\220\363\342\216\351\3126\362\340\207\355\324Z\364\345\236\365\346\240\364\344\227\362\340\214\362\340\210\361\336\203\361\335~\350\307+\360\333x\353\317I\362\337\205\347\305$\356\327h\351\3127\361\335\177\360\333u\356\326d\355\324_\346\303\036\355\324]\347\306'\357\330l\356\326c\354\322V\354\321R\343\300\031\346\303\032\355\324[\354\321Q\354\321M\353\317K\340\276\031~\253 \212\000\000\000\032tRNS\000\000\002*\005\003B\n\004\001@7|\217\220\210\251X6y\213\202R\017\b\r61\331w\000\000\000\255IDAT\030\323]\314\351\026\201`\024\205\341\203(\363<m*2\317\221\2201s\344\376\257G\237,,\317\277\363\256\275\016y\301\3708?\275\241\304 \300\013\202?\370\ne\246\364\335A\024%Q\222\334]\210s\202\\\301\227SPu\277\270\020&(\250\325\352b\243\331jw\272=D\b}\014\206\315\221\254\216'\332TG\2240C\273\243\216\347\232^Q\026\006b\204%\252+M_+\306f\2733Y\330C9\034\214\243\331?\235/g\026\256\260,\347\270\335m\373a#N>\374J$\211K\2453\037\331\\\236(X(~\360\002\221\347\317\023ms\034T\267\373^\262\000\000\000\000IEND\256B`\202", 527);
		_img_tree_folder_on = new wxBitmap(wxImage(sm));
	}
	{
		wxMemoryInputStream sm("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000@\000\000\000@\b\003\000\000\000\235\267\201\354\000\000\002\342PLTE\377\377\377\000\000\000\216\216\216qqq\000\000\000\r\r\r\017\017\017\017\017\017\017\017\017\020\020\020\r\r\r\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\001\001\001\002\002\002\002\002\002\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\001\001\001\001\001\001\002\002\002\002\002\002\002\002\002\002\002\002\003\003\003\000\000\000\027\027\027\004\004\004\014\014\014   \252\252\252\026\026\026%%%@@@@@@AAAAAA@@@BBBH@H\000\000\000\002\002\002\001\001\001\023\023\023\022\022\022\232'%\371\000\000\000\361tRNS\000\001\011\011\003M\342\326\331\346c\254\321\006\002!\357\374\371P\004o\374\373\217|\200\177\334\267\302p\37586l\b\005\035\353\376\233\211\355\036\232\357\022\033\034\r\330\245;\372}\356\350MN\032 \037\300\335\017i\243\014\"\345\364\361\363(a\371bL\230\202\201\343\324\352\207u\340\3062\327\270g\312\342\020F\aW\235\204\274\231\276?\247S`^]\025.\367\255\3173I/\370\226)\261\266\260\262\347rH_\275\024\344\206GQ\011\323\240\n\315\366\365w\313\341\023401\220\027cA,YV\\*\250\331\026\346\322zv:\325\332\030\251\265\271OEBC>\210\354+\241\351\0319%\216n\234{Z\013\326\303\225[\336\021U7t\320\307\304\221\214\337\256j-\301\252\227\263&\224h\310=D\336\310\373\375\355\354\373k,\366\350\030\003\261\226\004(f^_d \302\025'~\000\000\004\313IDATX\303\355VkXTU\024\335\227n\331\303\271T\336\356$&\216\305\300\305\a`\344\000\206\231\344+\241\aa$I!\022b\n1*\201\211\004\226D\017\260B\020\211\250|\225dF =\314^fQai\357wj\357\262\254\026\326\377\3669\0143\303\b\337L\375\253\2175\363}w\346\234{\366Y{\355\265\317\275D\203\030\304\377\025\312\200\bp\375\277\232\362FP\320\011\375#(\220\325*\235x\322\220\223\373\305\220SN\345i\377\001N\303\200\030j\361\233\205F\301\247w\237\321\335?\316<6\314/\005\235\316\202a={x\310\b\037\204\204\014?\a\030\311;\370+A(\020\332\377\334(Xm\243{((\232\033\276\031\234kXq\036\205\351\307\301N\341\021&\"\375\344\240\323\030\330\306\216\023Ri}!\344\035\0173*X&\021=.f\202\013>\011\234\037\013\\ v\361U[\341\301\211p`\004o\242S\234-B\">*\301\247\206\223`\305\205\222fb\360d7.\n\216\221\371M\001.\236J\212NI\356\312\306\373\004\270\004\210\235\306\032\321\364\0313gypi\302l\322tJF\n.#]\245\313a\275\"\356\312\324\324\253\220&\223u\323\214\231c\342jI2\035&L7\254\210#]\241\350k\200\271\232\242R\206\003\243\305\212k1\217,\236\322\352\224\011\303\221\301\003\212~\035\214>&\274>KP\037\303\024\346S6\305D`\001\205Q\360X\344\320\rSr\027\332{\213\230\013\344\361E\245E\254\305\215\213\227\344\367\242`\311M\"\261B\247\201\245\314\324\262\014\311\274\365r'3+\342\360\223\244l\032\335\\l \223\211\250T\002#eE?\215r\013\014\347\312\322U\245e\270\225\003\224\233\250 \232\a\254\226i\350\224\nsN\"\037(t\333\355\302\266\026o#)2\3000\330dF&\326p&\3710+\211\356\340\352V\362\234BUw\002w\361/\235*8\327\002\276\222\352\206\256\011\225\357\276\a\316jF\rJ8\300ZT\027\022\331\357\025\273\251\374\231\317\211\027\361U\243\221\300}\367\373\232I\021\337\360\332uu\214u\265\2239~=\312\326\363\245\201)l U\247H \255Jh\325Xm\240^\020\320\312\037hZ \321\364`sO\004oE\036\302\262la\337\207\201R\276\375\221\215@\2524A\210\354;\215\325\336\344)\344f\021\221t\273\013:\023-\305\\\273\320n\013\034\266\255\304\211\033\316B\261\254*M\264\264\"\312\365\250k\265Q\214t\021\300\233\201N9xl\232\030\333V\006\264\020=\016l\227&(\342\234\222\344~\332\023\311\005\371\371\025\005\r\033Ml\221#\265;6<)\320\372T\266Jmh\017\227\221\332`5v\206;mh\220&\350\000\236~\246\317f\317\2326c+\233^x\317\221\342\340/fD\213#\306Y'\r\364\334,\033v=\017\241)\233`\002\037\034\273]\a\207*,\220M-@.\337\247\323\013\\\337\036\2441\203\027\245\017\204\234\221\260\305\263wvK\011\2238\203Va\014\227\205,4=\312D>\251\n\275\3642\360\312\236\234\234=C\021\252\353Ti\303\253==\260\334a\262P\330!\355P\316vH\026\267\273\221\016\354}\215\270\013_\207\255f\266\030)\301*5\214:\213\361\206\253\242KY\005\274\231-K\255\274\005t1\223E\013w\325\013\354{\373\035\240\215D\335V\002\373\331\341\026e?\306\363\272\254v\354s\005h\205\265X\364\274\324\364\000\254\346D\242N\217\a\254\316w\205\204\031\016C\234W\212\242\346\341\275\244\364Q\233\333\361~o#\347\001\316f\331S\n\255\347\262~@v\332\356\211\360\241l\221z`S\225\3209;\255w\242\253\327\346\0375-nu\033\344c a\033Q]fE\262D\305'\315\302`\302\252kyZ\241\254\275F\315L\376T[c=\215\342\365p\350\344\207\303\001\262\367\355$Q\036\233\353X\267\257h\334)\321\350>44\356XO\004>\231\272\024M\267\330-\022v\213\312\317\"\365S\240#\200\347\263\320\264\200+\371\231\357\360\347<\330\352\n0\320\243\255\227n\326\027\307\32015<\321\013\341Y-\350\376R\013\354%E\2475\370\352\340\241\303\207\016\273\301\177\016~\215od\241\375C\243o\277c\035}\340\300\367?\004\372\232\364#\375\364\347\361\370\353g:\022\330z\246\360\313\257G\177\363\301\321\337\377\b\224\300\200/t\001\257g\016G\372\301?X?\210A\374\327\3607\b,\202\nq^MA\000\000\000\000IEND\256B`\202", 2287);
		_img_vroomgis = new wxBitmap(wxImage(sm));
	}
	{
		wxMemoryInputStream sm("\211PNG\r\n\032\n\000\000\000\rIHDR\000\000\000 \000\000\000 \b\003\000\000\000D\244\212\306\000\000\002\rPLTE\377\377\377\000\000\000\020\020\020\017\017\017\n\n\n\004\004\004HHH\036\036\036\011\011\011\003\003\003}}}rrrgggiiilllqqq\011\011\011\027\027\027\027\027\027\006\006\006\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\003\003\003\000\000\000\000\000\000\001\001\001888+++\030\030\030CCCAAABCBFEF\030\026\030\025\025\025\024\024\024\000\000\000\013\013\013\014\014\014\017\017\017\261\355\376\320\000\000\000\251tRNS\000\000\000\000\000\000\000\000\000\000\n\n\b\b\n\0149\350\357G\250\321\310\314\312\313\n\336\2562_q1\263*\216\235\244\232{\315\023\004\201\227\223\221\231L\330\027J\352\017\035\277P\354\011`\212\3643\254\322\r\200\301U/9B\006\366kj\376G\266X\214\367\001\032\324\202F\325\270\337\224zd7A?>&!#$\"\a\353W|\241)\350\355\303\357\363\335=\243;EN\251\267om\265,YO\345\343\346\304\361\372aV\203\341f\236\342\242\036\371\306\002:\020\026@\033\b\205\246\261\240\177\220\350\210\211\334n_\005;:;:\003\342 \037\274\000\000\001\255IDAT8\313\335\221i;\224a\030\206\337\253HY\212dD\332PD\r\"\302\250P3\366e\324\250$\373Z\366\261\245R\022\011\221%-4\225\3355\365\033\273\337\031f\034\306/p~x><\317y_\307{_\257\242\034\b\260\315!O/o\237m\216\2378\274s\r\207\340\353\267\271\345\340\244\277\253p\212\001\232@;\247\203\030\344\"\004\363\014\234\204\360\354^\341\034\317\357\022.\360\"\020\032\026~\351r\304\216\020\311+\210\212\276zM\253\325\306\304\002q\274\216x&\004\220\312\r\333{\"\223\200\233\264\223\234\202T\352\200\264[\320\245)\267\357\244\213\220\301x \223w\357\351\005C\n\262\262\231\223\233\227\217\2028\245\220E\"$\027\033Q\302\373\316\257x\240F\231P\372P\301#\006C\317\307@\031\237\300X\376\264\242R_%FuMm]=\032t\n\032\331\204f>\003\236\347It\213:\331\352\bj\213\226-\332\332\323\213;\200Nv\001Fswe\0173\201\374\336\276\202\256nt\364\212\320O\r_\000\003|i\237z\305\327\300 \337\274\225\350!\215\bY\3578,\313\206\323\014\323\373\302\221\221\354\026\300\300Q\340\303\030>\216\253E\225qB\316II\036\265\365 \235\016QF\370iJ\212\222\247\351\3173jt+g\215\206\271\2129s\024\3469\000|Y\370\372\355\373\017\305\271\272Im\317\316\"\313]~7\340\266\304\237\226\"\301b\341/\367}\204#\277\255\326\2776\254\377\376x\354#\034]^YY\265\261\266\276q\314!\034\b\376\003\221t\215\011\201h1m\000\000\000\000IEND\256B`\202", 1204);
		_img_vroomgis_sml = new wxBitmap(wxImage(sm));
	}
	return;
}


void vroomgis_clear_images(void){
	wxDELETE(_img_vroomgis);
	wxDELETE(_img_vroomgis_sml);
    wxDELETE(_img_cursor_editing);
	wxDELETE(_img_cursor_zoomin);
	wxDELETE(_img_cursor_zoomout);
    wxDELETE(_img_tree_folder_on);
    wxDELETE(_img_tree_folder_off);
}

