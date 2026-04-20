#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xc609ff70, "strncpy" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x37031a65, "__register_chrdev" },
	{ 0x653aa194, "class_create" },
	{ 0xe486c4b7, "device_create" },
	{ 0x1595e410, "device_destroy" },
	{ 0xa1dacb42, "class_destroy" },
	{ 0x52b15b3b, "__unregister_chrdev" },
	{ 0xd272d446, "__fentry__" },
	{ 0xbd03ed67, "__ref_stack_chk_guard" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0xbebe66ff, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xc609ff70,
	0xe8213e80,
	0xd272d446,
	0xd272d446,
	0x37031a65,
	0x653aa194,
	0xe486c4b7,
	0x1595e410,
	0xa1dacb42,
	0x52b15b3b,
	0xd272d446,
	0xbd03ed67,
	0x092a35a2,
	0xbebe66ff,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"strncpy\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"__stack_chk_fail\0"
	"__register_chrdev\0"
	"class_create\0"
	"device_create\0"
	"device_destroy\0"
	"class_destroy\0"
	"__unregister_chrdev\0"
	"__fentry__\0"
	"__ref_stack_chk_guard\0"
	"_copy_from_user\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "76007BED03732A614C9F108");
