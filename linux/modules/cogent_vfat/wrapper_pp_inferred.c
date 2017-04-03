/*
This build info header is now disabled by --fno-gen-header.
--------------------------------------------------------------------------------
We strongly discourage users from generating individual files for Isabelle
proofs, as it will end up with an inconsistent collection of output files (i.e.
Isabelle input files).
*/

#include <adt.h>
#include <wrapper.h>
#include <fat.h>
#include <wraphead.h>
typedef struct super_block *Superblock;
typedef void *SysState;
typedef struct inode *VfsInode;
typedef void *ErrPtr;
typedef struct dentry *VfsDentry;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef struct unit_t {
            int dummy;
        } unit_t;
typedef struct bool_t {
            u8 boolean;
        } bool_t;
enum {
    LET_TRUE = 1,
} ;
enum {
    LETBANG_TRUE = 1,
} ;
enum tag_t {
    TAG_ENUM_Error,
    TAG_ENUM_Next,
    TAG_ENUM_None,
    TAG_ENUM_Return,
    TAG_ENUM_Some,
    TAG_ENUM_Stop,
    TAG_ENUM_Success,
    TAG_ENUM_Yield,
} ;
typedef enum tag_t tag_t;
enum untyped_func_enum {
    FUN_ENUM_add_head_cog,
    FUN_ENUM_alias_cond,
    FUN_ENUM_brelse_ac,
    FUN_ENUM_clear_nlink_ac,
    FUN_ENUM_d_find_alias_ac,
    FUN_ENUM_d_inode_ac,
    FUN_ENUM_d_instantiate_ac,
    FUN_ENUM_d_move_ac,
    FUN_ENUM_d_splice_alias_ac,
    FUN_ENUM_d_unhashed_ac,
    FUN_ENUM_decrementU32,
    FUN_ENUM_deep_fat_remove_entries_ac,
    FUN_ENUM_deep_fat_write_inode_ac,
    FUN_ENUM_del_slots_cog,
    FUN_ENUM_del_slots_cons,
    FUN_ENUM_del_slots_gen,
    FUN_ENUM_delete_first_ac,
    FUN_ENUM_dput_ac,
    FUN_ENUM_drop_nlink_ac,
    FUN_ENUM_fat_add_entries_ac,
    FUN_ENUM_fat_alloc_new_dir_ac,
    FUN_ENUM_fat_attach_cog,
    FUN_ENUM_fat_build_inode_cog,
    FUN_ENUM_fat_detach_cog,
    FUN_ENUM_fat_dir_empty_ac,
    FUN_ENUM_fat_dir_hash_cog,
    FUN_ENUM_fat_fill_inode_ac,
    FUN_ENUM_fat_free_clusters_ac,
    FUN_ENUM_fat_hash_cog,
    FUN_ENUM_fat_iget_ac,
    FUN_ENUM_fat_msg_ac,
    FUN_ENUM_fat_remove_entries_cog,
    FUN_ENUM_fat_search_long_ac,
    FUN_ENUM_fat_sync_inode_cog,
    FUN_ENUM_flock_buildinode,
    FUN_ENUM_funlock_buildinode,
    FUN_ENUM_get_b_data,
    FUN_ENUM_get_current_time,
    FUN_ENUM_get_dhead,
    FUN_ENUM_get_dir_hash,
    FUN_ENUM_get_fat_hash,
    FUN_ENUM_get_head,
    FUN_ENUM_get_logstart,
    FUN_ENUM_get_name,
    FUN_ENUM_get_null_inode_ac,
    FUN_ENUM_get_parent,
    FUN_ENUM_get_qstr_length,
    FUN_ENUM_get_qstr_name,
    FUN_ENUM_get_sb,
    FUN_ENUM_get_version,
    FUN_ENUM_hash_32_ac_0,
    FUN_ENUM_hash_32_ac_1,
    FUN_ENUM_hlist_add_head_ac,
    FUN_ENUM_hlist_del_init_ac,
    FUN_ENUM_inc_nlink_ac,
    FUN_ENUM_incr_head_pointer,
    FUN_ENUM_incr_version,
    FUN_ENUM_insert_inode_hash_ac,
    FUN_ENUM_iput_ac,
    FUN_ENUM_is_dir_ac,
    FUN_ENUM_is_dirsync_ac,
    FUN_ENUM_is_disconnected,
    FUN_ENUM_is_equal_den,
    FUN_ENUM_is_err,
    FUN_ENUM_is_nfs,
    FUN_ENUM_is_noent,
    FUN_ENUM_is_null_0,
    FUN_ENUM_is_null_1,
    FUN_ENUM_is_root_den,
    FUN_ENUM_is_root_ino,
    FUN_ENUM_iterate_0,
    FUN_ENUM_iterate_1,
    FUN_ENUM_last_char_dot_ac,
    FUN_ENUM_mark_buffer_dirty_inode_ac,
    FUN_ENUM_mark_inode_dirty_ac,
    FUN_ENUM_new_inode_ac,
    FUN_ENUM_noent_ac,
    FUN_ENUM_ptr_greq_ac,
    FUN_ENUM_set_d_time,
    FUN_ENUM_set_ino,
    FUN_ENUM_set_ipos,
    FUN_ENUM_set_nlink_ac,
    FUN_ENUM_set_time,
    FUN_ENUM_set_version,
    FUN_ENUM_setup_inode,
    FUN_ENUM_slock,
    FUN_ENUM_spdir_lock,
    FUN_ENUM_spdir_unlock,
    FUN_ENUM_spinode_lock,
    FUN_ENUM_spinode_unlock,
    FUN_ENUM_striptail_cons,
    FUN_ENUM_striptail_gen,
    FUN_ENUM_sync_dirty_buffer_ac,
    FUN_ENUM_sync_inode_cog,
    FUN_ENUM_ulock,
    FUN_ENUM_update_am_time,
    FUN_ENUM_vfat_add_entry_cog,
    FUN_ENUM_vfat_build_slots_ac,
    FUN_ENUM_vfat_create_cog,
    FUN_ENUM_vfat_d_anon_disconn_cog,
    FUN_ENUM_vfat_find_cog,
    FUN_ENUM_vfat_lookup_cog,
    FUN_ENUM_vfat_mkdir_cog,
    FUN_ENUM_vfat_rmdir_cog,
    FUN_ENUM_vfat_striptail_len_cog,
    FUN_ENUM_vfat_unlink_cog,
} ;
typedef enum untyped_func_enum untyped_func_enum;
typedef untyped_func_enum t90;
typedef untyped_func_enum t91;
typedef untyped_func_enum t92;
typedef untyped_func_enum t93;
typedef untyped_func_enum t94;
typedef untyped_func_enum t95;
typedef untyped_func_enum t96;
typedef untyped_func_enum t97;
typedef untyped_func_enum t98;
typedef untyped_func_enum t99;
typedef untyped_func_enum t100;
typedef untyped_func_enum t101;
typedef untyped_func_enum t102;
typedef untyped_func_enum t103;
typedef untyped_func_enum t104;
typedef untyped_func_enum t105;
typedef untyped_func_enum t106;
typedef untyped_func_enum t107;
typedef untyped_func_enum t108;
typedef untyped_func_enum t109;
typedef untyped_func_enum t110;
typedef untyped_func_enum t111;
typedef untyped_func_enum t112;
typedef untyped_func_enum t113;
typedef untyped_func_enum t114;
typedef untyped_func_enum t115;
typedef untyped_func_enum t116;
typedef untyped_func_enum t117;
typedef untyped_func_enum t118;
typedef untyped_func_enum t119;
typedef untyped_func_enum t120;
typedef untyped_func_enum t38;
typedef untyped_func_enum t42;
typedef untyped_func_enum t121;
typedef untyped_func_enum t122;
typedef untyped_func_enum t123;
typedef untyped_func_enum t124;
typedef untyped_func_enum t73;
typedef untyped_func_enum t49;
typedef untyped_func_enum t125;
typedef untyped_func_enum t126;
typedef untyped_func_enum t127;
typedef untyped_func_enum t53;
typedef untyped_func_enum t78;
typedef untyped_func_enum t128;
typedef untyped_func_enum t129;
typedef untyped_func_enum t130;
typedef untyped_func_enum t131;
typedef untyped_func_enum t132;
typedef untyped_func_enum t133;
typedef untyped_func_enum t134;
typedef untyped_func_enum t135;
typedef untyped_func_enum t136;
typedef untyped_func_enum t137;
typedef untyped_func_enum t138;
typedef untyped_func_enum t139;
typedef untyped_func_enum t140;
typedef untyped_func_enum t141;
typedef untyped_func_enum t142;
typedef untyped_func_enum t143;
typedef untyped_func_enum t144;
typedef untyped_func_enum t145;
typedef untyped_func_enum t146;
typedef untyped_func_enum t147;
typedef untyped_func_enum t148;
typedef untyped_func_enum t149;
typedef untyped_func_enum t150;
struct t1 {
    u64 p1;
    u64 p2;
} ;
typedef struct t1 t1;
struct t2 {
    char *p1;
    u32 p2;
} ;
typedef struct t2 t2;
struct t3 {
    VfsInode *p1;
    u32 p2;
} ;
typedef struct t3 t3;
struct t4 {
    VfsInode *p1;
    u64 p2;
} ;
typedef struct t4 t4;
struct t5 {
    VfsInode *p1;
    VfsDentry *p2;
} ;
typedef struct t5 t5;
struct t6 {
    VfsDentry *p1;
    VfsDentry *p2;
} ;
typedef struct t6 t6;
struct t7 {
    VfsDentry *p1;
    u64 p2;
} ;
typedef struct t7 t7;
struct t8 {
    u32 seconds;
    u32 nanoseconds;
} ;
typedef struct t8 t8;
struct t9 {
    VfsInode *p1;
    t8 p2;
} ;
typedef struct t9 t9;
struct t10 {
    SysState *p1;
    t8 p2;
} ;
typedef struct t10 t10;
struct t11 {
    SysState *p1;
    Superblock *p2;
    char *p3;
} ;
typedef struct t11 t11;
struct t12 {
    SysState *p1;
    Superblock *p2;
} ;
typedef struct t12 t12;
struct t13 {
    VfsInode *p1;
    Superblock *p2;
} ;
typedef struct t13 t13;
struct t14 {
    VfsInode *p1;
    u64 p2;
    u32 p3;
} ;
typedef struct t14 t14;
struct t15 {
    tag_t tag;
    VfsInode *Error;
    VfsInode *Success;
} ;
typedef struct t15 t15;
struct t16 {
    u64 i_pos;
    u64 slot_off;
    u32 nr_slots;
    u64 de;
    u64 bh;
} ;
typedef struct t16 t16;
struct t17 {
    tag_t tag;
    ErrPtr *Error;
    t16 Success;
} ;
typedef struct t17 t17;
struct t18 {
    VfsInode *p1;
    t17 p2;
} ;
typedef struct t18 t18;
struct t19 {
    tag_t tag;
    ErrPtr *Error;
    u32 Success;
} ;
typedef struct t19 t19;
struct t20 {
    VfsInode *p1;
    t19 p2;
} ;
typedef struct t20 t20;
struct t21 {
    tag_t tag;
    ErrPtr *Error;
    unit_t Success;
} ;
typedef struct t21 t21;
struct t22 {
    VfsInode *p1;
    ErrPtr *p2;
} ;
typedef struct t22 t22;
struct t23 {
    tag_t tag;
    t22 Error;
    VfsInode *Success;
} ;
typedef struct t23 t23;
struct t24 {
    Superblock *p1;
    u64 p2;
} ;
typedef struct t24 t24;
struct t25 {
    Superblock *p1;
    t21 p2;
} ;
typedef struct t25 t25;
struct t26 {
    tag_t tag;
    ErrPtr *Error;
    VfsInode *Success;
} ;
typedef struct t26 t26;
struct t27 {
    Superblock *p1;
    t26 p2;
} ;
typedef struct t27 t27;
struct t28 {
    u64 p1;
    t21 p2;
} ;
typedef struct t28 t28;
struct t29 {
    VfsInode *p1;
    u64 p2;
    u32 p3;
    u32 p4;
    u32 p5;
    t8 p6;
} ;
typedef struct t29 t29;
struct t30 {
    u64 p1;
    u32 p2;
} ;
typedef struct t30 t30;
struct t31 {
    tag_t tag;
    ErrPtr *Error;
    t30 Success;
} ;
typedef struct t31 t31;
struct t32 {
    VfsInode *p1;
    t31 p2;
} ;
typedef struct t32 t32;
struct t33 {
    u32 p1;
    u64 p2;
    u64 p3;
} ;
typedef struct t33 t33;
struct t34 {
    t33 acc;
    unit_t obsv;
} ;
typedef struct t34 t34;
struct t35 {
    u32 p1;
    u64 p2;
} ;
typedef struct t35 t35;
struct t36 {
    tag_t tag;
    unit_t Return;
    t35 Stop;
    t35 Yield;
} ;
typedef struct t36 t36;
struct t37 {
    t33 p1;
    t36 p2;
} ;
typedef struct t37 t37;
struct t39 {
    t35 obj;
    t33 acc;
    unit_t obsv;
} ;
typedef struct t39 t39;
struct t40 {
    tag_t tag;
    unit_t Next;
    unit_t Return;
    t35 Stop;
} ;
typedef struct t40 t40;
struct t41 {
    t33 p1;
    t40 p2;
} ;
typedef struct t41 t41;
struct t43 {
    t38 gen;
    t42 cons;
    t33 acc;
    unit_t obsv;
} ;
typedef struct t43 t43;
struct t44 {
    tag_t tag;
    unit_t Return;
    t35 Stop;
} ;
typedef struct t44 t44;
struct t45 {
    t33 p1;
    t44 p2;
} ;
typedef struct t45 t45;
struct t46 {
    u32 acc;
    char *obsv;
} ;
typedef struct t46 t46;
struct t47 {
    tag_t tag;
    unit_t Return;
    u32 Stop;
    u32 Yield;
} ;
typedef struct t47 t47;
struct t48 {
    u32 p1;
    t47 p2;
} ;
typedef struct t48 t48;
struct t50 {
    u32 obj;
    u32 acc;
    char *obsv;
} ;
typedef struct t50 t50;
struct t51 {
    tag_t tag;
    unit_t Next;
    unit_t Return;
    u32 Stop;
} ;
typedef struct t51 t51;
struct t52 {
    u32 p1;
    t51 p2;
} ;
typedef struct t52 t52;
struct t54 {
    t49 gen;
    t53 cons;
    u32 acc;
    char *obsv;
} ;
typedef struct t54 t54;
struct t55 {
    tag_t tag;
    unit_t Return;
    u32 Stop;
} ;
typedef struct t55 t55;
struct t56 {
    u32 p1;
    t55 p2;
} ;
typedef struct t56 t56;
struct t57 {
    VfsInode *p1;
    VfsDentry *p2;
    t8 p3;
} ;
typedef struct t57 t57;
struct t58 {
    SysState *p1;
    VfsInode *p2;
    u64 p3;
} ;
typedef struct t58 t58;
struct t59 {
    SysState *p1;
    VfsInode *p2;
} ;
typedef struct t59 t59;
struct t60 {
    tag_t tag;
    t5 Error;
    t5 Success;
} ;
typedef struct t60 t60;
struct t61 {
    tag_t tag;
    t5 Error;
} ;
typedef struct t61 t61;
struct t62 {
    tag_t tag;
    t5 Success;
} ;
typedef struct t62 t62;
struct t63 {
    tag_t tag;
    t35 Yield;
} ;
typedef struct t63 t63;
struct t64 {
    tag_t tag;
    t35 Stop;
} ;
typedef struct t64 t64;
struct t65 {
    tag_t tag;
    u32 Yield;
} ;
typedef struct t65 t65;
struct t66 {
    tag_t tag;
    u32 Stop;
} ;
typedef struct t66 t66;
struct t67 {
    SysState *p1;
    Superblock *p2;
    t16 p3;
} ;
typedef struct t67 t67;
struct t68 {
    t12 p1;
    t26 p2;
} ;
typedef struct t68 t68;
struct t69 {
    tag_t tag;
    VfsInode *Success;
} ;
typedef struct t69 t69;
struct t70 {
    tag_t tag;
    ErrPtr *Error;
} ;
typedef struct t70 t70;
struct t71 {
    tag_t tag;
    t22 Error;
} ;
typedef struct t71 t71;
struct t72 {
    tag_t tag;
    unit_t Next;
} ;
typedef struct t72 t72;
struct t74 {
    SysState *p1;
    VfsInode *p2;
    t16 p3;
} ;
typedef struct t74 t74;
struct t75 {
    t59 p1;
    t21 p2;
} ;
typedef struct t75 t75;
struct t76 {
    tag_t tag;
    unit_t Success;
} ;
typedef struct t76 t76;
struct t77 {
    tag_t tag;
    VfsInode *Error;
} ;
typedef struct t77 t77;
struct t79 {
    tag_t tag;
    u32 Success;
} ;
typedef struct t79 t79;
struct t80 {
    VfsInode *p1;
    u64 p2;
    u32 p3;
    u32 p4;
    t8 p5;
} ;
typedef struct t80 t80;
struct t81 {
    tag_t tag;
    t16 Success;
} ;
typedef struct t81 t81;
struct t82 {
    SysState *p1;
    VfsInode *p2;
    VfsDentry *p3;
} ;
typedef struct t82 t82;
struct t83 {
    t82 p1;
    t26 p2;
} ;
typedef struct t83 t83;
struct t84 {
    tag_t tag;
    unit_t None;
    VfsDentry *Some;
} ;
typedef struct t84 t84;
struct t85 {
    tag_t tag;
    ErrPtr *Error;
    t84 Success;
} ;
typedef struct t85 t85;
struct t86 {
    t82 p1;
    t85 p2;
} ;
typedef struct t86 t86;
struct t87 {
    tag_t tag;
    VfsDentry *Some;
} ;
typedef struct t87 t87;
struct t88 {
    tag_t tag;
    t84 Success;
} ;
typedef struct t88 t88;
struct t89 {
    tag_t tag;
    unit_t None;
} ;
typedef struct t89 t89;
bool_t ptr_greq_ac(t1);
bool_t last_char_dot_ac(t2);
bool_t is_null_0(VfsInode *);
bool_t is_null_1(VfsDentry *);
u64 incr_head_pointer(t1);
unit_t hlist_del_init_ac(u64);
u64 hash_32_ac_1(u64);
u64 hash_32_ac_0(u32);
char *get_qstr_name(u64);
u32 get_qstr_length(u64);
u64 get_b_data(u64);
u64 delete_first_ac(u64);
u32 decrementU32(u32);
u64 brelse_ac(u64);
VfsInode *clear_nlink_ac(VfsInode *);
VfsInode *deep_fat_write_inode_ac(t3);
VfsInode *drop_nlink_ac(VfsInode *);
VfsInode *fat_free_clusters_ac(t3);
u64 get_dhead(VfsInode *);
u64 get_dir_hash(VfsInode *);
u64 get_fat_hash(VfsInode *);
u64 get_head(VfsInode *);
t3 get_logstart(VfsInode *);
VfsInode *get_null_inode_ac(unit_t);
u64 get_version(VfsInode *);
VfsInode *hlist_add_head_ac(t4);
VfsInode *inc_nlink_ac(VfsInode *);
VfsInode *insert_inode_hash_ac(VfsInode *);
unit_t iput_ac(VfsInode *);
bool_t is_dir_ac(VfsInode *);
bool_t is_dirsync_ac(VfsInode *);
bool_t is_nfs(VfsInode *);
bool_t is_root_ino(VfsInode *);
t4 mark_buffer_dirty_inode_ac(t4);
VfsInode *mark_inode_dirty_ac(VfsInode *);
VfsInode *set_ipos(t4);
VfsInode *set_nlink_ac(t3);
VfsInode *set_version(t4);
VfsInode *update_am_time(VfsInode *);
t5 d_find_alias_ac(VfsInode *);
VfsInode *d_inode_ac(VfsDentry *);
t5 d_instantiate_ac(t5);
t6 d_move_ac(t6);
VfsDentry *d_splice_alias_ac(t5);
VfsDentry *d_unhashed_ac(VfsDentry *);
unit_t dput_ac(VfsDentry *);
u64 get_name(VfsDentry *);
VfsDentry *get_parent(VfsDentry *);
bool_t is_disconnected(VfsDentry *);
bool_t is_equal_den(t6);
bool_t is_root_den(VfsDentry *);
VfsDentry *set_d_time(t7);
VfsInode *set_time(t9);
t10 get_current_time(SysState *);
SysState *fat_msg_ac(t11);
t12 flock_buildinode(t12);
t12 funlock_buildinode(t12);
Superblock *get_sb(VfsInode *);
t13 set_ino(t13);
t12 slock(t12);
t12 spdir_lock(t12);
SysState *spdir_unlock(t12);
t12 spinode_lock(t12);
SysState *spinode_unlock(t12);
SysState *ulock(t12);
t15 deep_fat_remove_entries_ac(t14);
t18 fat_add_entries_ac(t14);
t20 fat_alloc_new_dir_ac(t9);
t21 fat_dir_empty_ac(VfsInode *);
t23 fat_fill_inode_ac(t4);
t25 fat_iget_ac(t24);
t18 fat_search_long_ac(t14);
t26 is_err(VfsInode *);
t21 is_noent(ErrPtr *);
t27 new_inode_ac(Superblock *);
ErrPtr *noent_ac(unit_t);
t28 sync_dirty_buffer_ac(u64);
t32 vfat_build_slots_ac(t29);
static inline t45 iterate_1(t43);
static inline t56 iterate_0(t54);
__attribute__((const)) u64 fat_hash_cog(u64);
VfsInode *fat_sync_inode_cog(VfsInode *);
t4 fat_dir_hash_cog(VfsInode *);
VfsInode *incr_version(VfsInode *);
VfsInode *sync_inode_cog(VfsInode *);
__attribute__((pure)) bool_t vfat_d_anon_disconn_cog(VfsDentry *);
t5 setup_inode(t57);
t59 add_head_cog(t58);
t59 fat_attach_cog(t58);
t59 fat_detach_cog(t59);
t60 alias_cond(t5);
__attribute__((const)) t37 del_slots_gen(t34);
__attribute__((const)) t48 striptail_gen(t46);
t68 fat_build_inode_cog(t67);
__attribute__((const)) t41 del_slots_cons(t39);
__attribute__((const)) t52 striptail_cons(t50);
__attribute__((const)) t30 del_slots_cog(t33);
t75 fat_remove_entries_cog(t74);
t19 vfat_striptail_len_cog(u64);
t18 vfat_add_entry_cog(t80);
t83 vfat_create_cog(t82);
t83 vfat_mkdir_cog(t82);
t18 vfat_find_cog(t4);
t86 vfat_lookup_cog(t82);
t83 vfat_rmdir_cog(t82);
t83 vfat_unlink_cog(t82);
static inline t21 dispatch_t90(untyped_func_enum a2, ErrPtr *a3)
{
    return is_noent(a3);
}
static inline t27 dispatch_t91(untyped_func_enum a2, Superblock *a3)
{
    return new_inode_ac(a3);
}
static inline t10 dispatch_t92(untyped_func_enum a2, SysState *a3)
{
    return get_current_time(a3);
}
static inline VfsDentry *dispatch_t93(untyped_func_enum a2, VfsDentry *a3)
{
    switch (a2) {
        
      case FUN_ENUM_d_unhashed_ac:
        return d_unhashed_ac(a3);
        
      default:
        return get_parent(a3);
    }
}
static inline VfsInode *dispatch_t94(untyped_func_enum a2, VfsDentry *a3)
{
    return d_inode_ac(a3);
}
static inline bool_t dispatch_t95(untyped_func_enum a2, VfsDentry *a3)
{
    switch (a2) {
        
      case FUN_ENUM_is_disconnected:
        return is_disconnected(a3);
        
      case FUN_ENUM_is_null_1:
        return is_null_1(a3);
        
      case FUN_ENUM_is_root_den:
        return is_root_den(a3);
        
      default:
        return vfat_d_anon_disconn_cog(a3);
    }
}
static inline u64 dispatch_t96(untyped_func_enum a2, VfsDentry *a3)
{
    return get_name(a3);
}
static inline unit_t dispatch_t97(untyped_func_enum a2, VfsDentry *a3)
{
    return dput_ac(a3);
}
static inline Superblock *dispatch_t98(untyped_func_enum a2, VfsInode *a3)
{
    return get_sb(a3);
}
static inline VfsInode *dispatch_t99(untyped_func_enum a2, VfsInode *a3)
{
    switch (a2) {
        
      case FUN_ENUM_clear_nlink_ac:
        return clear_nlink_ac(a3);
        
      case FUN_ENUM_drop_nlink_ac:
        return drop_nlink_ac(a3);
        
      case FUN_ENUM_fat_sync_inode_cog:
        return fat_sync_inode_cog(a3);
        
      case FUN_ENUM_inc_nlink_ac:
        return inc_nlink_ac(a3);
        
      case FUN_ENUM_incr_version:
        return incr_version(a3);
        
      case FUN_ENUM_insert_inode_hash_ac:
        return insert_inode_hash_ac(a3);
        
      case FUN_ENUM_mark_inode_dirty_ac:
        return mark_inode_dirty_ac(a3);
        
      case FUN_ENUM_sync_inode_cog:
        return sync_inode_cog(a3);
        
      default:
        return update_am_time(a3);
    }
}
static inline bool_t dispatch_t100(untyped_func_enum a2, VfsInode *a3)
{
    switch (a2) {
        
      case FUN_ENUM_is_dir_ac:
        return is_dir_ac(a3);
        
      case FUN_ENUM_is_dirsync_ac:
        return is_dirsync_ac(a3);
        
      case FUN_ENUM_is_nfs:
        return is_nfs(a3);
        
      case FUN_ENUM_is_null_0:
        return is_null_0(a3);
        
      default:
        return is_root_ino(a3);
    }
}
static inline t21 dispatch_t101(untyped_func_enum a2, VfsInode *a3)
{
    return fat_dir_empty_ac(a3);
}
static inline t26 dispatch_t102(untyped_func_enum a2, VfsInode *a3)
{
    return is_err(a3);
}
static inline t3 dispatch_t103(untyped_func_enum a2, VfsInode *a3)
{
    return get_logstart(a3);
}
static inline t4 dispatch_t104(untyped_func_enum a2, VfsInode *a3)
{
    return fat_dir_hash_cog(a3);
}
static inline t5 dispatch_t105(untyped_func_enum a2, VfsInode *a3)
{
    return d_find_alias_ac(a3);
}
static inline u64 dispatch_t106(untyped_func_enum a2, VfsInode *a3)
{
    switch (a2) {
        
      case FUN_ENUM_get_dhead:
        return get_dhead(a3);
        
      case FUN_ENUM_get_dir_hash:
        return get_dir_hash(a3);
        
      case FUN_ENUM_get_fat_hash:
        return get_fat_hash(a3);
        
      case FUN_ENUM_get_head:
        return get_head(a3);
        
      default:
        return get_version(a3);
    }
}
static inline unit_t dispatch_t107(untyped_func_enum a2, VfsInode *a3)
{
    return iput_ac(a3);
}
static inline bool_t dispatch_t108(untyped_func_enum a2, t1 a3)
{
    return ptr_greq_ac(a3);
}
static inline u64 dispatch_t109(untyped_func_enum a2, t1 a3)
{
    return incr_head_pointer(a3);
}
static inline SysState *dispatch_t110(untyped_func_enum a2, t11 a3)
{
    return fat_msg_ac(a3);
}
static inline SysState *dispatch_t111(untyped_func_enum a2, t12 a3)
{
    switch (a2) {
        
      case FUN_ENUM_spdir_unlock:
        return spdir_unlock(a3);
        
      case FUN_ENUM_spinode_unlock:
        return spinode_unlock(a3);
        
      default:
        return ulock(a3);
    }
}
static inline t12 dispatch_t112(untyped_func_enum a2, t12 a3)
{
    switch (a2) {
        
      case FUN_ENUM_flock_buildinode:
        return flock_buildinode(a3);
        
      case FUN_ENUM_funlock_buildinode:
        return funlock_buildinode(a3);
        
      case FUN_ENUM_slock:
        return slock(a3);
        
      case FUN_ENUM_spdir_lock:
        return spdir_lock(a3);
        
      default:
        return spinode_lock(a3);
    }
}
static inline t13 dispatch_t113(untyped_func_enum a2, t13 a3)
{
    return set_ino(a3);
}
static inline t15 dispatch_t114(untyped_func_enum a2, t14 a3)
{
    return deep_fat_remove_entries_ac(a3);
}
static inline t18 dispatch_t115(untyped_func_enum a2, t14 a3)
{
    switch (a2) {
        
      case FUN_ENUM_fat_add_entries_ac:
        return fat_add_entries_ac(a3);
        
      default:
        return fat_search_long_ac(a3);
    }
}
static inline bool_t dispatch_t116(untyped_func_enum a2, t2 a3)
{
    return last_char_dot_ac(a3);
}
static inline t25 dispatch_t117(untyped_func_enum a2, t24 a3)
{
    return fat_iget_ac(a3);
}
static inline t32 dispatch_t118(untyped_func_enum a2, t29 a3)
{
    return vfat_build_slots_ac(a3);
}
static inline VfsInode *dispatch_t119(untyped_func_enum a2, t3 a3)
{
    switch (a2) {
        
      case FUN_ENUM_deep_fat_write_inode_ac:
        return deep_fat_write_inode_ac(a3);
        
      case FUN_ENUM_fat_free_clusters_ac:
        return fat_free_clusters_ac(a3);
        
      default:
        return set_nlink_ac(a3);
    }
}
static inline t30 dispatch_t120(untyped_func_enum a2, t33 a3)
{
    return del_slots_cog(a3);
}
static inline t37 dispatch_t38(untyped_func_enum a2, t34 a3)
{
    return del_slots_gen(a3);
}
static inline t41 dispatch_t42(untyped_func_enum a2, t39 a3)
{
    return del_slots_cons(a3);
}
static inline VfsInode *dispatch_t121(untyped_func_enum a2, t4 a3)
{
    switch (a2) {
        
      case FUN_ENUM_hlist_add_head_ac:
        return hlist_add_head_ac(a3);
        
      case FUN_ENUM_set_ipos:
        return set_ipos(a3);
        
      default:
        return set_version(a3);
    }
}
static inline t18 dispatch_t122(untyped_func_enum a2, t4 a3)
{
    return vfat_find_cog(a3);
}
static inline t23 dispatch_t123(untyped_func_enum a2, t4 a3)
{
    return fat_fill_inode_ac(a3);
}
static inline t4 dispatch_t124(untyped_func_enum a2, t4 a3)
{
    return mark_buffer_dirty_inode_ac(a3);
}
static inline t45 dispatch_t73(untyped_func_enum a2, t43 a3)
{
    return iterate_1(a3);
}
static inline t48 dispatch_t49(untyped_func_enum a2, t46 a3)
{
    return striptail_gen(a3);
}
static inline VfsDentry *dispatch_t125(untyped_func_enum a2, t5 a3)
{
    return d_splice_alias_ac(a3);
}
static inline t5 dispatch_t126(untyped_func_enum a2, t5 a3)
{
    return d_instantiate_ac(a3);
}
static inline t60 dispatch_t127(untyped_func_enum a2, t5 a3)
{
    return alias_cond(a3);
}
static inline t52 dispatch_t53(untyped_func_enum a2, t50 a3)
{
    return striptail_cons(a3);
}
static inline t56 dispatch_t78(untyped_func_enum a2, t54 a3)
{
    return iterate_0(a3);
}
static inline t5 dispatch_t128(untyped_func_enum a2, t57 a3)
{
    return setup_inode(a3);
}
static inline t59 dispatch_t129(untyped_func_enum a2, t58 a3)
{
    switch (a2) {
        
      case FUN_ENUM_add_head_cog:
        return add_head_cog(a3);
        
      default:
        return fat_attach_cog(a3);
    }
}
static inline t59 dispatch_t130(untyped_func_enum a2, t59 a3)
{
    return fat_detach_cog(a3);
}
static inline bool_t dispatch_t131(untyped_func_enum a2, t6 a3)
{
    return is_equal_den(a3);
}
static inline t6 dispatch_t132(untyped_func_enum a2, t6 a3)
{
    return d_move_ac(a3);
}
static inline t68 dispatch_t133(untyped_func_enum a2, t67 a3)
{
    return fat_build_inode_cog(a3);
}
static inline VfsDentry *dispatch_t134(untyped_func_enum a2, t7 a3)
{
    return set_d_time(a3);
}
static inline t75 dispatch_t135(untyped_func_enum a2, t74 a3)
{
    return fat_remove_entries_cog(a3);
}
static inline t18 dispatch_t136(untyped_func_enum a2, t80 a3)
{
    return vfat_add_entry_cog(a3);
}
static inline t83 dispatch_t137(untyped_func_enum a2, t82 a3)
{
    switch (a2) {
        
      case FUN_ENUM_vfat_create_cog:
        return vfat_create_cog(a3);
        
      case FUN_ENUM_vfat_mkdir_cog:
        return vfat_mkdir_cog(a3);
        
      case FUN_ENUM_vfat_rmdir_cog:
        return vfat_rmdir_cog(a3);
        
      default:
        return vfat_unlink_cog(a3);
    }
}
static inline t86 dispatch_t138(untyped_func_enum a2, t82 a3)
{
    return vfat_lookup_cog(a3);
}
static inline VfsInode *dispatch_t139(untyped_func_enum a2, t9 a3)
{
    return set_time(a3);
}
static inline t20 dispatch_t140(untyped_func_enum a2, t9 a3)
{
    return fat_alloc_new_dir_ac(a3);
}
static inline u32 dispatch_t141(untyped_func_enum a2, u32 a3)
{
    return decrementU32(a3);
}
static inline u64 dispatch_t142(untyped_func_enum a2, u32 a3)
{
    return hash_32_ac_0(a3);
}
static inline char *dispatch_t143(untyped_func_enum a2, u64 a3)
{
    return get_qstr_name(a3);
}
static inline t19 dispatch_t144(untyped_func_enum a2, u64 a3)
{
    return vfat_striptail_len_cog(a3);
}
static inline t28 dispatch_t145(untyped_func_enum a2, u64 a3)
{
    return sync_dirty_buffer_ac(a3);
}
static inline u32 dispatch_t146(untyped_func_enum a2, u64 a3)
{
    return get_qstr_length(a3);
}
static inline u64 dispatch_t147(untyped_func_enum a2, u64 a3)
{
    switch (a2) {
        
      case FUN_ENUM_brelse_ac:
        return brelse_ac(a3);
        
      case FUN_ENUM_delete_first_ac:
        return delete_first_ac(a3);
        
      case FUN_ENUM_fat_hash_cog:
        return fat_hash_cog(a3);
        
      case FUN_ENUM_get_b_data:
        return get_b_data(a3);
        
      default:
        return hash_32_ac_1(a3);
    }
}
static inline unit_t dispatch_t148(untyped_func_enum a2, u64 a3)
{
    return hlist_del_init_ac(a3);
}
static inline ErrPtr *dispatch_t149(untyped_func_enum a2, unit_t a3)
{
    return noent_ac(a3);
}
static inline VfsInode *dispatch_t150(untyped_func_enum a2, unit_t a3)
{
    return get_null_inode_ac(a3);
}
typedef u32 ErrCode;
typedef t16 SlotInfo;
typedef t8 Time;
typedef u32 WordArrayIndex;
typedef t58 add_head_cog_arg;
typedef t59 add_head_cog_ret;
typedef t5 alias_cond_arg;
typedef t60 alias_cond_ret;
typedef u64 brelse_ac_arg;
typedef u64 brelse_ac_ret;
typedef VfsInode *clear_nlink_ac_arg;
typedef VfsInode *clear_nlink_ac_ret;
typedef VfsInode *d_find_alias_ac_arg;
typedef t5 d_find_alias_ac_ret;
typedef VfsDentry *d_inode_ac_arg;
typedef VfsInode *d_inode_ac_ret;
typedef t5 d_instantiate_ac_arg;
typedef t5 d_instantiate_ac_ret;
typedef t6 d_move_ac_arg;
typedef t6 d_move_ac_ret;
typedef t5 d_splice_alias_ac_arg;
typedef VfsDentry *d_splice_alias_ac_ret;
typedef VfsDentry *d_unhashed_ac_arg;
typedef VfsDentry *d_unhashed_ac_ret;
typedef u32 decrementU32_arg;
typedef u32 decrementU32_ret;
typedef t14 deep_fat_remove_entries_ac_arg;
typedef t15 deep_fat_remove_entries_ac_ret;
typedef t3 deep_fat_write_inode_ac_arg;
typedef VfsInode *deep_fat_write_inode_ac_ret;
typedef t33 del_slots_cog_arg;
typedef t30 del_slots_cog_ret;
typedef t39 del_slots_cons_arg;
typedef t41 del_slots_cons_ret;
typedef t34 del_slots_gen_arg;
typedef t37 del_slots_gen_ret;
typedef u64 delete_first_ac_arg;
typedef u64 delete_first_ac_ret;
typedef VfsDentry *dput_ac_arg;
typedef unit_t dput_ac_ret;
typedef VfsInode *drop_nlink_ac_arg;
typedef VfsInode *drop_nlink_ac_ret;
typedef t14 fat_add_entries_ac_arg;
typedef t18 fat_add_entries_ac_ret;
typedef t9 fat_alloc_new_dir_ac_arg;
typedef t20 fat_alloc_new_dir_ac_ret;
typedef t58 fat_attach_cog_arg;
typedef t59 fat_attach_cog_ret;
typedef t67 fat_build_inode_cog_arg;
typedef t68 fat_build_inode_cog_ret;
typedef t59 fat_detach_cog_arg;
typedef t59 fat_detach_cog_ret;
typedef VfsInode *fat_dir_empty_ac_arg;
typedef t21 fat_dir_empty_ac_ret;
typedef VfsInode *fat_dir_hash_cog_arg;
typedef t4 fat_dir_hash_cog_ret;
typedef t4 fat_fill_inode_ac_arg;
typedef t23 fat_fill_inode_ac_ret;
typedef t3 fat_free_clusters_ac_arg;
typedef VfsInode *fat_free_clusters_ac_ret;
typedef u64 fat_hash_cog_arg;
typedef u64 fat_hash_cog_ret;
typedef t24 fat_iget_ac_arg;
typedef t25 fat_iget_ac_ret;
typedef t11 fat_msg_ac_arg;
typedef SysState *fat_msg_ac_ret;
typedef t74 fat_remove_entries_cog_arg;
typedef t75 fat_remove_entries_cog_ret;
typedef t14 fat_search_long_ac_arg;
typedef t18 fat_search_long_ac_ret;
typedef VfsInode *fat_sync_inode_cog_arg;
typedef VfsInode *fat_sync_inode_cog_ret;
typedef t12 flock_buildinode_arg;
typedef t12 flock_buildinode_ret;
typedef t12 funlock_buildinode_arg;
typedef t12 funlock_buildinode_ret;
typedef u64 get_b_data_arg;
typedef u64 get_b_data_ret;
typedef SysState *get_current_time_arg;
typedef t10 get_current_time_ret;
typedef VfsInode *get_dhead_arg;
typedef u64 get_dhead_ret;
typedef VfsInode *get_dir_hash_arg;
typedef u64 get_dir_hash_ret;
typedef VfsInode *get_fat_hash_arg;
typedef u64 get_fat_hash_ret;
typedef VfsInode *get_head_arg;
typedef u64 get_head_ret;
typedef VfsInode *get_logstart_arg;
typedef t3 get_logstart_ret;
typedef VfsDentry *get_name_arg;
typedef u64 get_name_ret;
typedef unit_t get_null_inode_ac_arg;
typedef VfsInode *get_null_inode_ac_ret;
typedef VfsDentry *get_parent_arg;
typedef VfsDentry *get_parent_ret;
typedef u64 get_qstr_length_arg;
typedef u32 get_qstr_length_ret;
typedef u64 get_qstr_name_arg;
typedef char *get_qstr_name_ret;
typedef VfsInode *get_sb_arg;
typedef Superblock *get_sb_ret;
typedef VfsInode *get_version_arg;
typedef u64 get_version_ret;
typedef u32 hash_32_ac_0_arg;
typedef u64 hash_32_ac_0_ret;
typedef u64 hash_32_ac_1_arg;
typedef u64 hash_32_ac_1_ret;
typedef t4 hlist_add_head_ac_arg;
typedef VfsInode *hlist_add_head_ac_ret;
typedef u64 hlist_del_init_ac_arg;
typedef unit_t hlist_del_init_ac_ret;
typedef VfsInode *inc_nlink_ac_arg;
typedef VfsInode *inc_nlink_ac_ret;
typedef t1 incr_head_pointer_arg;
typedef u64 incr_head_pointer_ret;
typedef VfsInode *incr_version_arg;
typedef VfsInode *incr_version_ret;
typedef VfsInode *insert_inode_hash_ac_arg;
typedef VfsInode *insert_inode_hash_ac_ret;
typedef VfsInode *iput_ac_arg;
typedef unit_t iput_ac_ret;
typedef VfsInode *is_dir_ac_arg;
typedef bool_t is_dir_ac_ret;
typedef VfsInode *is_dirsync_ac_arg;
typedef bool_t is_dirsync_ac_ret;
typedef VfsDentry *is_disconnected_arg;
typedef bool_t is_disconnected_ret;
typedef t6 is_equal_den_arg;
typedef bool_t is_equal_den_ret;
typedef VfsInode *is_err_arg;
typedef t26 is_err_ret;
typedef VfsInode *is_nfs_arg;
typedef bool_t is_nfs_ret;
typedef ErrPtr *is_noent_arg;
typedef t21 is_noent_ret;
typedef VfsInode *is_null_0_arg;
typedef bool_t is_null_0_ret;
typedef VfsDentry *is_null_1_arg;
typedef bool_t is_null_1_ret;
typedef VfsDentry *is_root_den_arg;
typedef bool_t is_root_den_ret;
typedef VfsInode *is_root_ino_arg;
typedef bool_t is_root_ino_ret;
typedef t54 iterate_0_arg;
typedef t56 iterate_0_ret;
typedef t43 iterate_1_arg;
typedef t45 iterate_1_ret;
typedef t2 last_char_dot_ac_arg;
typedef bool_t last_char_dot_ac_ret;
typedef t4 mark_buffer_dirty_inode_ac_arg;
typedef t4 mark_buffer_dirty_inode_ac_ret;
typedef VfsInode *mark_inode_dirty_ac_arg;
typedef VfsInode *mark_inode_dirty_ac_ret;
typedef Superblock *new_inode_ac_arg;
typedef t27 new_inode_ac_ret;
typedef unit_t noent_ac_arg;
typedef ErrPtr *noent_ac_ret;
typedef t1 ptr_greq_ac_arg;
typedef bool_t ptr_greq_ac_ret;
typedef t7 set_d_time_arg;
typedef VfsDentry *set_d_time_ret;
typedef t13 set_ino_arg;
typedef t13 set_ino_ret;
typedef t4 set_ipos_arg;
typedef VfsInode *set_ipos_ret;
typedef t3 set_nlink_ac_arg;
typedef VfsInode *set_nlink_ac_ret;
typedef t9 set_time_arg;
typedef VfsInode *set_time_ret;
typedef t4 set_version_arg;
typedef VfsInode *set_version_ret;
typedef t57 setup_inode_arg;
typedef t5 setup_inode_ret;
typedef t12 slock_arg;
typedef t12 slock_ret;
typedef t12 spdir_lock_arg;
typedef t12 spdir_lock_ret;
typedef t12 spdir_unlock_arg;
typedef SysState *spdir_unlock_ret;
typedef t12 spinode_lock_arg;
typedef t12 spinode_lock_ret;
typedef t12 spinode_unlock_arg;
typedef SysState *spinode_unlock_ret;
typedef t50 striptail_cons_arg;
typedef t52 striptail_cons_ret;
typedef t46 striptail_gen_arg;
typedef t48 striptail_gen_ret;
typedef u64 sync_dirty_buffer_ac_arg;
typedef t28 sync_dirty_buffer_ac_ret;
typedef VfsInode *sync_inode_cog_arg;
typedef VfsInode *sync_inode_cog_ret;
typedef t12 ulock_arg;
typedef SysState *ulock_ret;
typedef VfsInode *update_am_time_arg;
typedef VfsInode *update_am_time_ret;
typedef t80 vfat_add_entry_cog_arg;
typedef t18 vfat_add_entry_cog_ret;
typedef t29 vfat_build_slots_ac_arg;
typedef t32 vfat_build_slots_ac_ret;
typedef t82 vfat_create_cog_arg;
typedef t83 vfat_create_cog_ret;
typedef VfsDentry *vfat_d_anon_disconn_cog_arg;
typedef bool_t vfat_d_anon_disconn_cog_ret;
typedef t4 vfat_find_cog_arg;
typedef t18 vfat_find_cog_ret;
typedef t82 vfat_lookup_cog_arg;
typedef t86 vfat_lookup_cog_ret;
typedef t82 vfat_mkdir_cog_arg;
typedef t83 vfat_mkdir_cog_ret;
typedef t82 vfat_rmdir_cog_arg;
typedef t83 vfat_rmdir_cog_ret;
typedef u64 vfat_striptail_len_cog_arg;
typedef t19 vfat_striptail_len_cog_ret;
typedef t82 vfat_unlink_cog_arg;
typedef t83 vfat_unlink_cog_ret;
__attribute__((const)) u64 fat_hash_cog(u64 a1)
{
    u64 r2 = a1;
    u64 r3 = hash_32_ac_1(r2);
    
    return r3;
}
VfsInode *fat_sync_inode_cog(VfsInode *a1)
{
    VfsInode *r2 = a1;
    u8 r3 = 1U;
    u32 r4 = (u32) r3;
    t3 r5 = (t3) {.p1 = r2, .p2 = r4};
    VfsInode *r6 = deep_fat_write_inode_ac(r5);
    
    return r6;
}
t4 fat_dir_hash_cog(VfsInode *a1)
{
    VfsInode *r2 = a1;
    t3 r3 = get_logstart(r2);
    VfsInode *r4 = r3.p1;
    u32 r5 = r3.p2;
    u64 r6 = hash_32_ac_0(r5);
    t4 r7 = (t4) {.p1 = r4, .p2 = r6};
    
    return r7;
}
VfsInode *incr_version(VfsInode *a1)
{
    VfsInode *r2 = a1;
    u64 r3;
    
    if (LETBANG_TRUE)
        r3 = get_version(r2);
    else
        ;
    
    u8 r4 = 1U;
    u64 r5 = (u64) r4;
    u64 r6 = r3 + r5;
    t4 r7 = (t4) {.p1 = r2, .p2 = r6};
    VfsInode *r8 = set_version(r7);
    
    return r8;
}
VfsInode *sync_inode_cog(VfsInode *a1)
{
    VfsInode *r2 = a1;
    VfsInode *r3 = update_am_time(r2);
    bool_t r4;
    
    if (LETBANG_TRUE)
        r4 = is_dirsync_ac(r3);
    else
        ;
    
    bool_t r5 = (bool_t) {.boolean = 1U};
    bool_t r6 = (bool_t) {.boolean = r4.boolean == r5.boolean};
    VfsInode *r7;
    
    if (r6.boolean)
        r7 = fat_sync_inode_cog(r3);
    else {
        bool_t r8 = r4;
        
        r7 = mark_inode_dirty_ac(r3);
    }
    
    VfsInode *r9 = r7;
    
    return r9;
}
__attribute__((pure)) bool_t vfat_d_anon_disconn_cog(VfsDentry *a1)
{
    VfsDentry *r2 = a1;
    bool_t r3 = is_root_den(r2);
    bool_t r4 = (bool_t) {.boolean = 1U};
    bool_t r5 = (bool_t) {.boolean = r3.boolean == r4.boolean};
    bool_t r6;
    
    if (r5.boolean) {
        bool_t r7;
        
        if (LETBANG_TRUE)
            r7 = is_disconnected(r2);
        else
            ;
        
        bool_t r8 = (bool_t) {.boolean = 1U};
        bool_t r9 = (bool_t) {.boolean = r7.boolean == r8.boolean};
        bool_t r10;
        
        if (r9.boolean)
            r10 = (bool_t) {.boolean = 1U};
        else {
            bool_t r11 = r7;
            
            r10 = (bool_t) {.boolean = 0U};
        }
        r6 = r10;
    } else {
        bool_t r12 = r3;
        
        r6 = (bool_t) {.boolean = 0U};
    }
    
    bool_t r13 = r6;
    
    return r13;
}
t5 setup_inode(t57 a1)
{
    VfsInode *r2 = a1.p1;
    VfsDentry *r3 = a1.p2;
    t8 r4 = a1.p3;
    VfsInode *r5 = incr_version(r2);
    t9 r6 = (t9) {.p1 = r5, .p2 = r4};
    VfsInode *r7 = set_time(r6);
    t5 r8 = (t5) {.p1 = r7, .p2 = r3};
    t5 r9 = d_instantiate_ac(r8);
    VfsInode *r10 = r9.p1;
    VfsDentry *r11 = r9.p2;
    t5 r12 = (t5) {.p1 = r10, .p2 = r11};
    
    return r12;
}
t59 add_head_cog(t58 a1)
{
    SysState *r2 = a1.p1;
    VfsInode *r3 = a1.p2;
    u64 r4 = a1.p3;
    bool_t r5;
    
    if (LETBANG_TRUE)
        r5 = is_root_ino(r3);
    else
        ;
    
    bool_t r6 = (bool_t) {.boolean = 1U};
    bool_t r7 = (bool_t) {.boolean = r5.boolean == r6.boolean};
    t59 r8;
    
    if (r7.boolean)
        r8 = (t59) {.p1 = r2, .p2 = r3};
    else {
        bool_t r9 = r5;
        u64 r10;
        
        if (LETBANG_TRUE)
            r10 = get_head(r3);
        else
            ;
        
        u64 r11 = fat_hash_cog(r4);
        t1 r12 = (t1) {.p1 = r10, .p2 = r11};
        u64 r13 = incr_head_pointer(r12);
        Superblock *r14;
        
        if (LETBANG_TRUE)
            r14 = get_sb(r3);
        else
            ;
        
        t12 r15 = (t12) {.p1 = r2, .p2 = r14};
        t12 r16 = spinode_lock(r15);
        SysState *r17 = r16.p1;
        Superblock *r18 = r16.p2;
        t4 r19 = (t4) {.p1 = r3, .p2 = r4};
        VfsInode *r20 = set_ipos(r19);
        t4 r21 = (t4) {.p1 = r20, .p2 = r13};
        VfsInode *r22 = hlist_add_head_ac(r21);
        t12 r23 = (t12) {.p1 = r17, .p2 = r18};
        SysState *r24 = spinode_unlock(r23);
        
        r8 = (t59) {.p1 = r24, .p2 = r22};
    }
    
    t59 r25 = r8;
    
    return r25;
}
t59 fat_attach_cog(t58 a1)
{
    SysState *r2 = a1.p1;
    VfsInode *r3 = a1.p2;
    u64 r4 = a1.p3;
    t58 r5 = (t58) {.p1 = r2, .p2 = r3, .p3 = r4};
    t59 r6 = add_head_cog(r5);
    SysState *r7 = r6.p1;
    VfsInode *r8 = r6.p2;
    bool_t r9;
    
    if (LETBANG_TRUE)
        r9 = is_dir_ac(r8);
    else
        ;
    
    bool_t r10 = (bool_t) {.boolean = 1U};
    bool_t r11 = (bool_t) {.boolean = r9.boolean == r10.boolean};
    t59 r12;
    
    if (r11.boolean) {
        bool_t r13;
        
        if (LETBANG_TRUE)
            r13 = is_nfs(r8);
        else
            ;
        
        bool_t r14 = (bool_t) {.boolean = 1U};
        bool_t r15 = (bool_t) {.boolean = r13.boolean == r14.boolean};
        t59 r16;
        
        if (r15.boolean) {
            u64 r17;
            
            if (LETBANG_TRUE)
                r17 = get_dhead(r8);
            else
                ;
            
            t4 r18 = fat_dir_hash_cog(r8);
            VfsInode *r19 = r18.p1;
            u64 r20 = r18.p2;
            u64 r21 = r17 + r20;
            Superblock *r22;
            
            if (LETBANG_TRUE)
                r22 = get_sb(r19);
            else
                ;
            
            t12 r23 = (t12) {.p1 = r7, .p2 = r22};
            t12 r24 = spdir_lock(r23);
            SysState *r25 = r24.p1;
            Superblock *r26 = r24.p2;
            t4 r27 = (t4) {.p1 = r19, .p2 = r21};
            VfsInode *r28 = hlist_add_head_ac(r27);
            t12 r29 = (t12) {.p1 = r25, .p2 = r26};
            SysState *r30 = spdir_unlock(r29);
            
            r16 = (t59) {.p1 = r30, .p2 = r28};
        } else {
            bool_t r31 = r13;
            
            r16 = (t59) {.p1 = r7, .p2 = r8};
        }
        r12 = r16;
    } else {
        bool_t r32 = r9;
        
        r12 = (t59) {.p1 = r7, .p2 = r8};
    }
    
    t59 r33 = r12;
    
    return r33;
}
t59 fat_detach_cog(t59 a1)
{
    SysState *r2 = a1.p1;
    VfsInode *r3 = a1.p2;
    Superblock *r4;
    
    if (LETBANG_TRUE)
        r4 = get_sb(r3);
    else
        ;
    
    t12 r5 = (t12) {.p1 = r2, .p2 = r4};
    t12 r6 = spinode_lock(r5);
    SysState *r7 = r6.p1;
    Superblock *r8 = r6.p2;
    u64 r9;
    
    if (LETBANG_TRUE)
        r9 = get_fat_hash(r3);
    else
        ;
    
    u8 r10 = 0U;
    u64 r11 = (u64) r10;
    t4 r12 = (t4) {.p1 = r3, .p2 = r11};
    VfsInode *r13 = set_ipos(r12);
    unit_t r14 = hlist_del_init_ac(r9);
    t12 r15 = (t12) {.p1 = r7, .p2 = r8};
    SysState *r16 = spinode_unlock(r15);
    bool_t r17;
    
    if (LETBANG_TRUE)
        r17 = is_dir_ac(r13);
    else
        ;
    
    bool_t r18 = (bool_t) {.boolean = 1U};
    bool_t r19 = (bool_t) {.boolean = r17.boolean == r18.boolean};
    t59 r20;
    
    if (r19.boolean) {
        bool_t r21;
        
        if (LETBANG_TRUE)
            r21 = is_nfs(r13);
        else
            ;
        
        bool_t r22 = (bool_t) {.boolean = 1U};
        bool_t r23 = (bool_t) {.boolean = r21.boolean == r22.boolean};
        t59 r24;
        
        if (r23.boolean) {
            Superblock *r25;
            
            if (LETBANG_TRUE)
                r25 = get_sb(r13);
            else
                ;
            
            t12 r26 = (t12) {.p1 = r16, .p2 = r25};
            t12 r27 = spdir_lock(r26);
            SysState *r28 = r27.p1;
            Superblock *r29 = r27.p2;
            u64 r30;
            
            if (LETBANG_TRUE)
                r30 = get_dir_hash(r13);
            else
                ;
            
            unit_t r31 = hlist_del_init_ac(r30);
            t12 r32 = (t12) {.p1 = r28, .p2 = r29};
            SysState *r33 = spdir_unlock(r32);
            
            r24 = (t59) {.p1 = r33, .p2 = r13};
        } else {
            bool_t r34 = r21;
            
            r24 = (t59) {.p1 = r16, .p2 = r13};
        }
        r20 = r24;
    } else {
        bool_t r35 = r17;
        
        r20 = (t59) {.p1 = r16, .p2 = r13};
    }
    
    t59 r36 = r20;
    
    return r36;
}
t60 alias_cond(t5 a1)
{
    VfsInode *r2 = a1.p1;
    VfsDentry *r3 = a1.p2;
    t5 r4 = d_find_alias_ac(r2);
    VfsInode *r5 = r4.p1;
    VfsDentry *r6 = r4.p2;
    bool_t r7;
    
    if (LETBANG_TRUE)
        r7 = is_null_1(r6);
    else
        ;
    
    bool_t r8 = (bool_t) {.boolean = 1U};
    bool_t r9 = (bool_t) {.boolean = r7.boolean == r8.boolean};
    t60 r10;
    
    if (r9.boolean) {
        t5 r11 = (t5) {.p1 = r5, .p2 = r6};
        t61 r12 = (t61) {.tag = TAG_ENUM_Error, .Error = r11};
        
        r10 = (t60) {.tag = r12.tag, .Error = r12.Error};
    } else {
        bool_t r13 = r7;
        VfsDentry *r14;
        
        if (LETBANG_TRUE)
            r14 = get_parent(r6);
        else
            ;
        
        VfsDentry *r15;
        
        if (LETBANG_TRUE)
            r15 = get_parent(r3);
        else
            ;
        
        t6 r16 = (t6) {.p1 = r14, .p2 = r15};
        bool_t r17 = is_equal_den(r16);
        bool_t r18 = (bool_t) {.boolean = 1U};
        bool_t r19 = (bool_t) {.boolean = r17.boolean == r18.boolean};
        t60 r20;
        
        if (r19.boolean) {
            bool_t r21;
            
            if (LETBANG_TRUE)
                r21 = vfat_d_anon_disconn_cog(r6);
            else
                ;
            
            bool_t r22 = (bool_t) {.boolean = 1U};
            bool_t r23 = (bool_t) {.boolean = r21.boolean == r22.boolean};
            t60 r24;
            
            if (r23.boolean) {
                t5 r25 = (t5) {.p1 = r5, .p2 = r6};
                t61 r26 = (t61) {.tag = TAG_ENUM_Error, .Error = r25};
                
                r24 = (t60) {.tag = r26.tag, .Error = r26.Error};
            } else {
                bool_t r27 = r21;
                t5 r28 = (t5) {.p1 = r5, .p2 = r6};
                t62 r29 = (t62) {.tag = TAG_ENUM_Success, .Success = r28};
                
                r24 = (t60) {.tag = r29.tag, .Success = r29.Success};
            }
            r20 = r24;
        } else {
            bool_t r30 = r17;
            t5 r31 = (t5) {.p1 = r5, .p2 = r6};
            t61 r32 = (t61) {.tag = TAG_ENUM_Error, .Error = r31};
            
            r20 = (t60) {.tag = r32.tag, .Error = r32.Error};
        }
        r10 = r20;
    }
    
    t60 r33 = r10;
    
    return r33;
}
__attribute__((const)) t37 del_slots_gen(t34 a1)
{
    t33 r2 = a1.acc;
    u32 r3 = r2.p1;
    u64 r4 = r2.p2;
    u64 r5 = r2.p3;
    unit_t r6 = a1.obsv;
    unit_t r7 = r6;
    u64 r8 = get_b_data(r5);
    t1 r9 = (t1) {.p1 = r4, .p2 = r8};
    bool_t r10 = ptr_greq_ac(r9);
    u8 r11 = 0U;
    u32 r12 = (u32) r11;
    bool_t r13 = (bool_t) {.boolean = r3 > r12};
    bool_t r14 = (bool_t) {.boolean = r13.boolean && r10.boolean};
    bool_t r15 = (bool_t) {.boolean = 1U};
    bool_t r16 = (bool_t) {.boolean = r14.boolean == r15.boolean};
    t37 r17;
    
    if (r16.boolean) {
        t33 r18 = (t33) {.p1 = r3, .p2 = r4, .p3 = r5};
        t35 r19 = (t35) {.p1 = r3, .p2 = r4};
        t63 r20 = (t63) {.tag = TAG_ENUM_Yield, .Yield = r19};
        t36 r21 = (t36) {.tag = r20.tag, .Yield = r20.Yield};
        
        r17 = (t37) {.p1 = r18, .p2 = r21};
    } else {
        bool_t r22 = r14;
        t33 r23 = (t33) {.p1 = r3, .p2 = r4, .p3 = r5};
        t35 r24 = (t35) {.p1 = r3, .p2 = r4};
        t64 r25 = (t64) {.tag = TAG_ENUM_Stop, .Stop = r24};
        t36 r26 = (t36) {.tag = r25.tag, .Stop = r25.Stop};
        
        r17 = (t37) {.p1 = r23, .p2 = r26};
    }
    
    t37 r27 = r17;
    
    return r27;
}
__attribute__((const)) t48 striptail_gen(t46 a1)
{
    u32 r2 = a1.acc;
    char *r3 = a1.obsv;
    u8 r4 = 0U;
    u32 r5 = (u32) r4;
    bool_t r6 = (bool_t) {.boolean = r2 > r5};
    t2 r7 = (t2) {.p1 = r3, .p2 = r2};
    bool_t r8 = last_char_dot_ac(r7);
    bool_t r9 = (bool_t) {.boolean = r6.boolean && r8.boolean};
    t48 r10;
    
    if (r9.boolean) {
        t65 r11 = (t65) {.tag = TAG_ENUM_Yield, .Yield = r2};
        t47 r12 = (t47) {.tag = r11.tag, .Yield = r11.Yield};
        
        r10 = (t48) {.p1 = r2, .p2 = r12};
    } else {
        t66 r13 = (t66) {.tag = TAG_ENUM_Stop, .Stop = r2};
        t47 r14 = (t47) {.tag = r13.tag, .Stop = r13.Stop};
        
        r10 = (t48) {.p1 = r2, .p2 = r14};
    }
    
    t48 r15 = r10;
    
    return r15;
}
t68 fat_build_inode_cog(t67 a1)
{
    SysState *r2 = a1.p1;
    Superblock *r3 = a1.p2;
    t16 r4 = a1.p3;
    u64 r5 = r4.bh;
    u64 r6 = brelse_ac(r5);
    u64 r7 = r4.i_pos;
    u64 r8 = r4.de;
    t12 r9 = (t12) {.p1 = r2, .p2 = r3};
    t12 r10 = flock_buildinode(r9);
    SysState *r11 = r10.p1;
    Superblock *r12 = r10.p2;
    t24 r13 = (t24) {.p1 = r12, .p2 = r7};
    t25 r14 = fat_iget_ac(r13);
    Superblock *r15 = r14.p1;
    t21 r16 = r14.p2;
    t68 r17;
    
    if (r16.tag == TAG_ENUM_Success) {
        unit_t r18 = r16.Success;
        t27 r19 = new_inode_ac(r15);
        Superblock *r20 = r19.p1;
        t26 r21 = r19.p2;
        t68 r22;
        
        if (r21.tag == TAG_ENUM_Success) {
            t13 r23 = (t13) {.p1 = r21.Success, .p2 = r20};
            t13 r24 = set_ino(r23);
            VfsInode *r25 = r24.p1;
            Superblock *r26 = r24.p2;
            u8 r27 = 1U;
            u64 r28 = (u64) r27;
            t4 r29 = (t4) {.p1 = r25, .p2 = r28};
            VfsInode *r30 = set_version(r29);
            t4 r31 = (t4) {.p1 = r30, .p2 = r8};
            t23 r32 = fat_fill_inode_ac(r31);
            t68 r33;
            
            if (r32.tag == TAG_ENUM_Success) {
                t58 r34 = (t58) {.p1 = r11, .p2 = r32.Success, .p3 = r7};
                t59 r35 = fat_attach_cog(r34);
                SysState *r36 = r35.p1;
                VfsInode *r37 = r35.p2;
                VfsInode *r38 = insert_inode_hash_ac(r37);
                t12 r39 = (t12) {.p1 = r36, .p2 = r26};
                t12 r40 = funlock_buildinode(r39);
                SysState *r41 = r40.p1;
                Superblock *r42 = r40.p2;
                t26 r43 = is_err(r38);
                t68 r44;
                
                if (r43.tag == TAG_ENUM_Success) {
                    t12 r45 = (t12) {.p1 = r41, .p2 = r42};
                    t69 r46 = (t69) {.tag = TAG_ENUM_Success, .Success =
                                     r43.Success};
                    t26 r47 = (t26) {.tag = r46.tag, .Success = r46.Success};
                    
                    r44 = (t68) {.p1 = r45, .p2 = r47};
                } else {
                    t70 r48 = {.tag =r43.tag, .Error =r43.Error};
                    ErrPtr *r49 = r48.Error;
                    t12 r50 = (t12) {.p1 = r41, .p2 = r42};
                    t70 r51 = (t70) {.tag = TAG_ENUM_Error, .Error = r49};
                    t26 r52 = (t26) {.tag = r51.tag, .Error = r51.Error};
                    
                    r44 = (t68) {.p1 = r50, .p2 = r52};
                }
                r33 = r44;
            } else {
                t71 r53 = {.tag =r32.tag, .Error =r32.Error};
                t22 r54 = r53.Error;
                VfsInode *r55 = r54.p1;
                ErrPtr *r56 = r54.p2;
                unit_t r57 = iput_ac(r55);
                t12 r58 = (t12) {.p1 = r11, .p2 = r26};
                t12 r59 = funlock_buildinode(r58);
                SysState *r60 = r59.p1;
                Superblock *r61 = r59.p2;
                t12 r62 = (t12) {.p1 = r60, .p2 = r61};
                t70 r63 = (t70) {.tag = TAG_ENUM_Error, .Error = r56};
                t26 r64 = (t26) {.tag = r63.tag, .Error = r63.Error};
                
                r33 = (t68) {.p1 = r62, .p2 = r64};
            }
            r22 = r33;
        } else {
            t70 r65 = {.tag =r21.tag, .Error =r21.Error};
            ErrPtr *r66 = r65.Error;
            t12 r67 = (t12) {.p1 = r11, .p2 = r20};
            t12 r68 = funlock_buildinode(r67);
            SysState *r69 = r68.p1;
            Superblock *r70 = r68.p2;
            t12 r71 = (t12) {.p1 = r69, .p2 = r70};
            t70 r72 = (t70) {.tag = TAG_ENUM_Error, .Error = r66};
            t26 r73 = (t26) {.tag = r72.tag, .Error = r72.Error};
            
            r22 = (t68) {.p1 = r71, .p2 = r73};
        }
        r17 = r22;
    } else {
        t70 r74 = {.tag =r16.tag, .Error =r16.Error};
        ErrPtr *r75 = r74.Error;
        t12 r76 = (t12) {.p1 = r11, .p2 = r15};
        t12 r77 = funlock_buildinode(r76);
        SysState *r78 = r77.p1;
        Superblock *r79 = r77.p2;
        t12 r80 = (t12) {.p1 = r78, .p2 = r79};
        t70 r81 = (t70) {.tag = TAG_ENUM_Error, .Error = r75};
        t26 r82 = (t26) {.tag = r81.tag, .Error = r81.Error};
        
        r17 = (t68) {.p1 = r80, .p2 = r82};
    }
    
    t68 r83 = r17;
    
    return r83;
}
__attribute__((const)) t41 del_slots_cons(t39 a1)
{
    t35 r2 = a1.obj;
    u32 r3 = r2.p1;
    u64 r4 = r2.p2;
    t33 r5 = a1.acc;
    u32 r6 = r5.p1;
    u64 r7 = r5.p2;
    u64 r8 = r5.p3;
    u32 r9 = r6;
    u64 r10 = r7;
    unit_t r11 = a1.obsv;
    unit_t r12 = r11;
    u64 r13 = delete_first_ac(r4);
    u32 r14 = decrementU32(r3);
    t33 r15 = (t33) {.p1 = r14, .p2 = r13, .p3 = r8};
    unit_t r16 = (unit_t) {.dummy = 0};
    t72 r17 = (t72) {.tag = TAG_ENUM_Next, .Next = r16};
    t40 r18 = (t40) {.tag = r17.tag, .Next = r17.Next};
    t41 r19 = (t41) {.p1 = r15, .p2 = r18};
    
    return r19;
}
__attribute__((const)) t52 striptail_cons(t50 a1)
{
    u32 r2 = a1.obj;
    u32 r3 = a1.acc;
    char *r4 = a1.obsv;
    u32 r5 = decrementU32(r2);
    unit_t r6 = (unit_t) {.dummy = 0};
    t72 r7 = (t72) {.tag = TAG_ENUM_Next, .Next = r6};
    t51 r8 = (t51) {.tag = r7.tag, .Next = r7.Next};
    t52 r9 = (t52) {.p1 = r5, .p2 = r8};
    
    return r9;
}
__attribute__((const)) t30 del_slots_cog(t33 a1)
{
    u32 r2 = a1.p1;
    u64 r3 = a1.p2;
    u64 r4 = a1.p3;
    t73 r5 = FUN_ENUM_iterate_1;
    t38 r6 = FUN_ENUM_del_slots_gen;
    t42 r7 = FUN_ENUM_del_slots_cons;
    t33 r8 = (t33) {.p1 = r2, .p2 = r4, .p3 = r3};
    unit_t r9 = (unit_t) {.dummy = 0};
    t43 r10 = (t43) {.gen = r6, .cons = r7, .acc = r8, .obsv = r9};
    t45 r11 = dispatch_t73(r5, r10);
    t33 r12 = r11.p1;
    t44 r13 = r11.p2;
    u32 r14 = r12.p1;
    u64 r15 = r12.p2;
    u64 r16 = r12.p3;
    u64 r17 = r16;
    t44 r18 = r13;
    t30 r19 = (t30) {.p1 = r15, .p2 = r14};
    
    return r19;
}
t75 fat_remove_entries_cog(t74 a1)
{
    SysState *r2 = a1.p1;
    VfsInode *r3 = a1.p2;
    t16 r4 = a1.p3;
    u32 r5 = r4.nr_slots;
    u64 r6 = r4.bh;
    u64 r7 = r4.de;
    t33 r8 = (t33) {.p1 = r5, .p2 = r6, .p3 = r7};
    t30 r9 = del_slots_cog(r8);
    u64 r10 = r9.p1;
    u32 r11 = r9.p2;
    u64 r12 = r4.bh;
    t4 r13 = (t4) {.p1 = r3, .p2 = r12};
    t4 r14 = mark_buffer_dirty_inode_ac(r13);
    VfsInode *r15 = r14.p1;
    u64 r16 = r14.p2;
    bool_t r17;
    
    if (LETBANG_TRUE)
        r17 = is_dirsync_ac(r15);
    else
        ;
    
    bool_t r18 = (bool_t) {.boolean = 1U};
    bool_t r19 = (bool_t) {.boolean = r17.boolean == r18.boolean};
    t75 r20;
    
    if (r19.boolean) {
        t28 r21 = sync_dirty_buffer_ac(r16);
        u64 r22 = r21.p1;
        t21 r23 = r21.p2;
        t75 r24;
        
        if (r23.tag == TAG_ENUM_Success) {
            unit_t r25 = r23.Success;
            u64 r26 = brelse_ac(r22);
            VfsInode *r27 = incr_version(r15);
            u8 r28 = 0U;
            u32 r29 = (u32) r28;
            bool_t r30 = (bool_t) {.boolean = r11 == r29};
            bool_t r31 = (bool_t) {.boolean = 1U};
            bool_t r32 = (bool_t) {.boolean = r30.boolean == r31.boolean};
            t75 r33;
            
            if (r32.boolean) {
                VfsInode *r34 = sync_inode_cog(r27);
                t59 r35 = (t59) {.p1 = r2, .p2 = r34};
                unit_t r36 = (unit_t) {.dummy = 0};
                t76 r37 = (t76) {.tag = TAG_ENUM_Success, .Success = r36};
                t21 r38 = (t21) {.tag = r37.tag, .Success = r37.Success};
                
                r33 = (t75) {.p1 = r35, .p2 = r38};
            } else {
                bool_t r39 = r30;
                u64 r40 = r4.slot_off;
                t14 r41 = (t14) {.p1 = r27, .p2 = r40, .p3 = r11};
                t15 r42 = deep_fat_remove_entries_ac(r41);
                t75 r43;
                
                if (r42.tag == TAG_ENUM_Success) {
                    VfsInode *r44 = sync_inode_cog(r42.Success);
                    t59 r45 = (t59) {.p1 = r2, .p2 = r44};
                    unit_t r46 = (unit_t) {.dummy = 0};
                    t76 r47 = (t76) {.tag = TAG_ENUM_Success, .Success = r46};
                    t21 r48 = (t21) {.tag = r47.tag, .Success = r47.Success};
                    
                    r43 = (t75) {.p1 = r45, .p2 = r48};
                } else {
                    t77 r49 = {.tag =r42.tag, .Error =r42.Error};
                    VfsInode *r50 = r49.Error;
                    Superblock *r51;
                    
                    if (LETBANG_TRUE)
                        r51 = get_sb(r50);
                    else
                        ;
                    
                    char *r52 = "Couldn't remove the long name slots";
                    t11 r53 = (t11) {.p1 = r2, .p2 = r51, .p3 = r52};
                    SysState *r54 = fat_msg_ac(r53);
                    VfsInode *r55 = sync_inode_cog(r50);
                    t59 r56 = (t59) {.p1 = r54, .p2 = r55};
                    unit_t r57 = (unit_t) {.dummy = 0};
                    t76 r58 = (t76) {.tag = TAG_ENUM_Success, .Success = r57};
                    t21 r59 = (t21) {.tag = r58.tag, .Success = r58.Success};
                    
                    r43 = (t75) {.p1 = r56, .p2 = r59};
                }
                r33 = r43;
            }
            r24 = r33;
        } else {
            t70 r60 = {.tag =r23.tag, .Error =r23.Error};
            ErrPtr *r61 = r60.Error;
            u64 r62 = brelse_ac(r22);
            t59 r63 = (t59) {.p1 = r2, .p2 = r15};
            t70 r64 = (t70) {.tag = TAG_ENUM_Error, .Error = r61};
            t21 r65 = (t21) {.tag = r64.tag, .Error = r64.Error};
            
            r24 = (t75) {.p1 = r63, .p2 = r65};
        }
        r20 = r24;
    } else {
        bool_t r66 = r17;
        u64 r67 = brelse_ac(r16);
        VfsInode *r68 = incr_version(r15);
        u8 r69 = 0U;
        u32 r70 = (u32) r69;
        bool_t r71 = (bool_t) {.boolean = r11 == r70};
        bool_t r72 = (bool_t) {.boolean = 1U};
        bool_t r73 = (bool_t) {.boolean = r71.boolean == r72.boolean};
        t75 r74;
        
        if (r73.boolean) {
            VfsInode *r75 = sync_inode_cog(r68);
            t59 r76 = (t59) {.p1 = r2, .p2 = r75};
            unit_t r77 = (unit_t) {.dummy = 0};
            t76 r78 = (t76) {.tag = TAG_ENUM_Success, .Success = r77};
            t21 r79 = (t21) {.tag = r78.tag, .Success = r78.Success};
            
            r74 = (t75) {.p1 = r76, .p2 = r79};
        } else {
            bool_t r80 = r71;
            u64 r81 = r4.slot_off;
            t14 r82 = (t14) {.p1 = r68, .p2 = r81, .p3 = r11};
            t15 r83 = deep_fat_remove_entries_ac(r82);
            t75 r84;
            
            if (r83.tag == TAG_ENUM_Success) {
                VfsInode *r85 = sync_inode_cog(r83.Success);
                t59 r86 = (t59) {.p1 = r2, .p2 = r85};
                unit_t r87 = (unit_t) {.dummy = 0};
                t76 r88 = (t76) {.tag = TAG_ENUM_Success, .Success = r87};
                t21 r89 = (t21) {.tag = r88.tag, .Success = r88.Success};
                
                r84 = (t75) {.p1 = r86, .p2 = r89};
            } else {
                t77 r90 = {.tag =r83.tag, .Error =r83.Error};
                VfsInode *r91 = r90.Error;
                Superblock *r92;
                
                if (LETBANG_TRUE)
                    r92 = get_sb(r91);
                else
                    ;
                
                char *r93 = "Couldn't remove the long name slots";
                t11 r94 = (t11) {.p1 = r2, .p2 = r92, .p3 = r93};
                SysState *r95 = fat_msg_ac(r94);
                VfsInode *r96 = sync_inode_cog(r91);
                t59 r97 = (t59) {.p1 = r95, .p2 = r96};
                unit_t r98 = (unit_t) {.dummy = 0};
                t76 r99 = (t76) {.tag = TAG_ENUM_Success, .Success = r98};
                t21 r100 = (t21) {.tag = r99.tag, .Success = r99.Success};
                
                r84 = (t75) {.p1 = r97, .p2 = r100};
            }
            r74 = r84;
        }
        r20 = r74;
    }
    
    t75 r101 = r20;
    
    return r101;
}
t19 vfat_striptail_len_cog(u64 a1)
{
    u64 r2 = a1;
    char *r3 = get_qstr_name(r2);
    u32 r4 = get_qstr_length(r2);
    t78 r5 = FUN_ENUM_iterate_0;
    t49 r6 = FUN_ENUM_striptail_gen;
    t53 r7 = FUN_ENUM_striptail_cons;
    t54 r8 = (t54) {.gen = r6, .cons = r7, .acc = r4, .obsv = r3};
    t56 r9 = dispatch_t78(r5, r8);
    u32 r10 = r9.p1;
    t55 r11 = r9.p2;
    t55 r12 = r11;
    u8 r13 = 0U;
    u32 r14 = (u32) r13;
    bool_t r15 = (bool_t) {.boolean = r10 == r14};
    bool_t r16 = (bool_t) {.boolean = 1U};
    bool_t r17 = (bool_t) {.boolean = r15.boolean == r16.boolean};
    t19 r18;
    
    if (r17.boolean) {
        unit_t r19 = (unit_t) {.dummy = 0};
        ErrPtr *r20 = noent_ac(r19);
        t70 r21 = (t70) {.tag = TAG_ENUM_Error, .Error = r20};
        
        r18 = (t19) {.tag = r21.tag, .Error = r21.Error};
    } else {
        bool_t r22 = r15;
        t79 r23 = (t79) {.tag = TAG_ENUM_Success, .Success = r10};
        
        r18 = (t19) {.tag = r23.tag, .Success = r23.Success};
    }
    
    t19 r24 = r18;
    
    return r24;
}
t18 vfat_add_entry_cog(t80 a1)
{
    VfsInode *r2 = a1.p1;
    u64 r3 = a1.p2;
    u32 r4 = a1.p3;
    u32 r5 = a1.p4;
    t8 r6 = a1.p5;
    t19 r7 = vfat_striptail_len_cog(r3);
    t18 r8;
    
    if (r7.tag == TAG_ENUM_Success) {
        t29 r9 = (t29) {.p1 = r2, .p2 = r3, .p3 = r7.Success, .p4 = r4, .p5 =
                        r5, .p6 = r6};
        t32 r10 = vfat_build_slots_ac(r9);
        VfsInode *r11 = r10.p1;
        t31 r12 = r10.p2;
        t18 r13;
        
        if (r12.tag == TAG_ENUM_Success) {
            u64 r14 = r12.Success.p1;
            u32 r15 = r12.Success.p2;
            t14 r16 = (t14) {.p1 = r11, .p2 = r14, .p3 = r15};
            t18 r17 = fat_add_entries_ac(r16);
            VfsInode *r18 = r17.p1;
            t17 r19 = r17.p2;
            t18 r20;
            
            if (r19.tag == TAG_ENUM_Success) {
                t9 r21 = (t9) {.p1 = r18, .p2 = r6};
                VfsInode *r22 = set_time(r21);
                bool_t r23;
                
                if (LETBANG_TRUE)
                    r23 = is_dirsync_ac(r22);
                else
                    ;
                
                bool_t r24 = (bool_t) {.boolean = 1U};
                bool_t r25 = (bool_t) {.boolean = r23.boolean == r24.boolean};
                t18 r26;
                
                if (r25.boolean) {
                    VfsInode *r27 = fat_sync_inode_cog(r22);
                    t81 r28 = (t81) {.tag = TAG_ENUM_Success, .Success =
                                     r19.Success};
                    t17 r29 = (t17) {.tag = r28.tag, .Success = r28.Success};
                    
                    r26 = (t18) {.p1 = r27, .p2 = r29};
                } else {
                    bool_t r30 = r23;
                    VfsInode *r31 = mark_inode_dirty_ac(r22);
                    t81 r32 = (t81) {.tag = TAG_ENUM_Success, .Success =
                                     r19.Success};
                    t17 r33 = (t17) {.tag = r32.tag, .Success = r32.Success};
                    
                    r26 = (t18) {.p1 = r31, .p2 = r33};
                }
                r20 = r26;
            } else {
                t70 r34 = {.tag =r19.tag, .Error =r19.Error};
                ErrPtr *r35 = r34.Error;
                t70 r36 = (t70) {.tag = TAG_ENUM_Error, .Error = r35};
                t17 r37 = (t17) {.tag = r36.tag, .Error = r36.Error};
                
                r20 = (t18) {.p1 = r18, .p2 = r37};
            }
            r13 = r20;
        } else {
            t70 r38 = {.tag =r12.tag, .Error =r12.Error};
            ErrPtr *r39 = r38.Error;
            t70 r40 = (t70) {.tag = TAG_ENUM_Error, .Error = r39};
            t17 r41 = (t17) {.tag = r40.tag, .Error = r40.Error};
            
            r13 = (t18) {.p1 = r11, .p2 = r41};
        }
        r8 = r13;
    } else {
        t70 r42 = {.tag =r7.tag, .Error =r7.Error};
        ErrPtr *r43 = r42.Error;
        t70 r44 = (t70) {.tag = TAG_ENUM_Error, .Error = r43};
        t17 r45 = (t17) {.tag = r44.tag, .Error = r44.Error};
        
        r8 = (t18) {.p1 = r2, .p2 = r45};
    }
    
    t18 r46 = r8;
    
    return r46;
}
t83 vfat_create_cog(t82 a1)
{
    SysState *r2 = a1.p1;
    VfsInode *r3 = a1.p2;
    VfsDentry *r4 = a1.p3;
    Superblock *r5;
    
    if (LETBANG_TRUE)
        r5 = get_sb(r3);
    else
        ;
    
    t12 r6 = (t12) {.p1 = r2, .p2 = r5};
    t12 r7 = slock(r6);
    SysState *r8 = r7.p1;
    Superblock *r9 = r7.p2;
    t10 r10 = get_current_time(r8);
    SysState *r11 = r10.p1;
    t8 r12 = r10.p2;
    u64 r13;
    
    if (LETBANG_TRUE)
        r13 = get_name(r4);
    else
        ;
    
    u8 r14 = 0U;
    u32 r15 = (u32) r14;
    u8 r16 = 0U;
    u32 r17 = (u32) r16;
    t80 r18 = (t80) {.p1 = r3, .p2 = r13, .p3 = r15, .p4 = r17, .p5 = r12};
    t18 r19 = vfat_add_entry_cog(r18);
    VfsInode *r20 = r19.p1;
    t17 r21 = r19.p2;
    t83 r22;
    
    if (r21.tag == TAG_ENUM_Success) {
        VfsInode *r23 = incr_version(r20);
        t67 r24 = (t67) {.p1 = r11, .p2 = r9, .p3 = r21.Success};
        t68 r25 = fat_build_inode_cog(r24);
        t12 r26 = r25.p1;
        t26 r27 = r25.p2;
        SysState *r28 = r26.p1;
        Superblock *r29 = r26.p2;
        t83 r30;
        
        if (r27.tag == TAG_ENUM_Success) {
            t57 r31 = (t57) {.p1 = r27.Success, .p2 = r4, .p3 = r12};
            t5 r32 = setup_inode(r31);
            VfsInode *r33 = r32.p1;
            VfsDentry *r34 = r32.p2;
            t12 r35 = (t12) {.p1 = r28, .p2 = r29};
            SysState *r36 = ulock(r35);
            t82 r37 = (t82) {.p1 = r36, .p2 = r23, .p3 = r34};
            t69 r38 = (t69) {.tag = TAG_ENUM_Success, .Success = r33};
            t26 r39 = (t26) {.tag = r38.tag, .Success = r38.Success};
            
            r30 = (t83) {.p1 = r37, .p2 = r39};
        } else {
            t70 r40 = {.tag =r27.tag, .Error =r27.Error};
            ErrPtr *r41 = r40.Error;
            t12 r42 = (t12) {.p1 = r28, .p2 = r29};
            SysState *r43 = ulock(r42);
            t82 r44 = (t82) {.p1 = r43, .p2 = r23, .p3 = r4};
            t70 r45 = (t70) {.tag = TAG_ENUM_Error, .Error = r41};
            t26 r46 = (t26) {.tag = r45.tag, .Error = r45.Error};
            
            r30 = (t83) {.p1 = r44, .p2 = r46};
        }
        r22 = r30;
    } else {
        t70 r47 = {.tag =r21.tag, .Error =r21.Error};
        ErrPtr *r48 = r47.Error;
        t12 r49 = (t12) {.p1 = r11, .p2 = r9};
        SysState *r50 = ulock(r49);
        t82 r51 = (t82) {.p1 = r50, .p2 = r20, .p3 = r4};
        t70 r52 = (t70) {.tag = TAG_ENUM_Error, .Error = r48};
        t26 r53 = (t26) {.tag = r52.tag, .Error = r52.Error};
        
        r22 = (t83) {.p1 = r51, .p2 = r53};
    }
    
    t83 r54 = r22;
    
    return r54;
}
t83 vfat_mkdir_cog(t82 a1)
{
    SysState *r2 = a1.p1;
    VfsInode *r3 = a1.p2;
    VfsDentry *r4 = a1.p3;
    Superblock *r5;
    
    if (LETBANG_TRUE)
        r5 = get_sb(r3);
    else
        ;
    
    t10 r6 = get_current_time(r2);
    SysState *r7 = r6.p1;
    t8 r8 = r6.p2;
    u64 r9;
    
    if (LETBANG_TRUE)
        r9 = get_name(r4);
    else
        ;
    
    t9 r10 = (t9) {.p1 = r3, .p2 = r8};
    t20 r11 = fat_alloc_new_dir_ac(r10);
    VfsInode *r12 = r11.p1;
    t19 r13 = r11.p2;
    t83 r14;
    
    if (r13.tag == TAG_ENUM_Success) {
        u8 r15 = 1U;
        u32 r16 = (u32) r15;
        t80 r17 = (t80) {.p1 = r12, .p2 = r9, .p3 = r16, .p4 = r13.Success,
                         .p5 = r8};
        t18 r18 = vfat_add_entry_cog(r17);
        VfsInode *r19 = r18.p1;
        t17 r20 = r18.p2;
        t83 r21;
        
        if (r20.tag == TAG_ENUM_Success) {
            VfsInode *r22 = incr_version(r19);
            VfsInode *r23 = inc_nlink_ac(r22);
            t67 r24 = (t67) {.p1 = r7, .p2 = r5, .p3 = r20.Success};
            t68 r25 = fat_build_inode_cog(r24);
            t12 r26 = r25.p1;
            t26 r27 = r25.p2;
            SysState *r28 = r26.p1;
            Superblock *r29 = r26.p2;
            t83 r30;
            
            if (r27.tag == TAG_ENUM_Success) {
                VfsInode *r31 = incr_version(r27.Success);
                u8 r32 = 2U;
                u32 r33 = (u32) r32;
                t3 r34 = (t3) {.p1 = r31, .p2 = r33};
                VfsInode *r35 = set_nlink_ac(r34);
                t9 r36 = (t9) {.p1 = r35, .p2 = r8};
                VfsInode *r37 = set_time(r36);
                t5 r38 = (t5) {.p1 = r37, .p2 = r4};
                t5 r39 = d_instantiate_ac(r38);
                VfsInode *r40 = r39.p1;
                VfsDentry *r41 = r39.p2;
                t12 r42 = (t12) {.p1 = r28, .p2 = r29};
                SysState *r43 = ulock(r42);
                t82 r44 = (t82) {.p1 = r43, .p2 = r23, .p3 = r41};
                t69 r45 = (t69) {.tag = TAG_ENUM_Success, .Success = r40};
                t26 r46 = (t26) {.tag = r45.tag, .Success = r45.Success};
                
                r30 = (t83) {.p1 = r44, .p2 = r46};
            } else {
                t70 r47 = {.tag =r27.tag, .Error =r27.Error};
                ErrPtr *r48 = r47.Error;
                t12 r49 = (t12) {.p1 = r28, .p2 = r29};
                SysState *r50 = ulock(r49);
                t82 r51 = (t82) {.p1 = r50, .p2 = r23, .p3 = r4};
                t70 r52 = (t70) {.tag = TAG_ENUM_Error, .Error = r48};
                t26 r53 = (t26) {.tag = r52.tag, .Error = r52.Error};
                
                r30 = (t83) {.p1 = r51, .p2 = r53};
            }
            r21 = r30;
        } else {
            t70 r54 = {.tag =r20.tag, .Error =r20.Error};
            ErrPtr *r55 = r54.Error;
            t3 r56 = (t3) {.p1 = r19, .p2 = r13.Success};
            VfsInode *r57 = fat_free_clusters_ac(r56);
            t12 r58 = (t12) {.p1 = r7, .p2 = r5};
            SysState *r59 = ulock(r58);
            t82 r60 = (t82) {.p1 = r59, .p2 = r57, .p3 = r4};
            t70 r61 = (t70) {.tag = TAG_ENUM_Error, .Error = r55};
            t26 r62 = (t26) {.tag = r61.tag, .Error = r61.Error};
            
            r21 = (t83) {.p1 = r60, .p2 = r62};
        }
        r14 = r21;
    } else {
        t70 r63 = {.tag =r13.tag, .Error =r13.Error};
        ErrPtr *r64 = r63.Error;
        t12 r65 = (t12) {.p1 = r7, .p2 = r5};
        SysState *r66 = ulock(r65);
        t82 r67 = (t82) {.p1 = r66, .p2 = r12, .p3 = r4};
        t70 r68 = (t70) {.tag = TAG_ENUM_Error, .Error = r64};
        t26 r69 = (t26) {.tag = r68.tag, .Error = r68.Error};
        
        r14 = (t83) {.p1 = r67, .p2 = r69};
    }
    
    t83 r70 = r14;
    
    return r70;
}
t18 vfat_find_cog(t4 a1)
{
    VfsInode *r2 = a1.p1;
    u64 r3 = a1.p2;
    t19 r4 = vfat_striptail_len_cog(r3);
    t18 r5;
    
    if (r4.tag == TAG_ENUM_Success) {
        t14 r6 = (t14) {.p1 = r2, .p2 = r3, .p3 = r4.Success};
        
        r5 = fat_search_long_ac(r6);
    } else {
        t70 r7 = {.tag =r4.tag, .Error =r4.Error};
        ErrPtr *r8 = r7.Error;
        t70 r9 = (t70) {.tag = TAG_ENUM_Error, .Error = r8};
        t17 r10 = (t17) {.tag = r9.tag, .Error = r9.Error};
        
        r5 = (t18) {.p1 = r2, .p2 = r10};
    }
    
    t18 r11 = r5;
    
    return r11;
}
t86 vfat_lookup_cog(t82 a1)
{
    SysState *r2 = a1.p1;
    VfsInode *r3 = a1.p2;
    VfsDentry *r4 = a1.p3;
    Superblock *r5;
    
    if (LETBANG_TRUE)
        r5 = get_sb(r3);
    else
        ;
    
    t12 r6 = (t12) {.p1 = r2, .p2 = r5};
    t12 r7 = slock(r6);
    SysState *r8 = r7.p1;
    Superblock *r9 = r7.p2;
    u64 r10;
    
    if (LETBANG_TRUE)
        r10 = get_name(r4);
    else
        ;
    
    t4 r11 = (t4) {.p1 = r3, .p2 = r10};
    t18 r12 = vfat_find_cog(r11);
    VfsInode *r13 = r12.p1;
    t17 r14 = r12.p2;
    t86 r15;
    
    if (r14.tag == TAG_ENUM_Success) {
        t67 r16 = (t67) {.p1 = r8, .p2 = r9, .p3 = r14.Success};
        t68 r17 = fat_build_inode_cog(r16);
        t12 r18 = r17.p1;
        t26 r19 = r17.p2;
        SysState *r20 = r18.p1;
        Superblock *r21 = r18.p2;
        t86 r22;
        
        if (r19.tag == TAG_ENUM_Success) {
            t60 r23;
            
            if (LETBANG_TRUE) {
                t5 r24 = (t5) {.p1 = r19.Success, .p2 = r4};
                
                r23 = alias_cond(r24);
            } else
                ;
            
            t86 r25;
            
            if (r23.tag == TAG_ENUM_Success) {
                VfsInode *r26 = r23.Success.p1;
                VfsDentry *r27 = r23.Success.p2;
                VfsDentry *r28 = d_unhashed_ac(r27);
                bool_t r29;
                
                if (LETBANG_TRUE)
                    r29 = is_dir_ac(r26);
                else
                    ;
                
                bool_t r30 = (bool_t) {.boolean = 1U};
                bool_t r31 = (bool_t) {.boolean = r29.boolean == r30.boolean};
                t86 r32;
                
                if (r31.boolean) {
                    unit_t r33 = iput_ac(r26);
                    t12 r34 = (t12) {.p1 = r20, .p2 = r21};
                    SysState *r35 = ulock(r34);
                    t82 r36 = (t82) {.p1 = r35, .p2 = r13, .p3 = r4};
                    t87 r37 = (t87) {.tag = TAG_ENUM_Some, .Some = r28};
                    t84 r38 = (t84) {.tag = r37.tag, .Some = r37.Some};
                    t88 r39 = (t88) {.tag = TAG_ENUM_Success, .Success = r38};
                    t85 r40 = (t85) {.tag = r39.tag, .Success = r39.Success};
                    
                    r32 = (t86) {.p1 = r36, .p2 = r40};
                } else {
                    bool_t r41 = r29;
                    t6 r42 = (t6) {.p1 = r28, .p2 = r4};
                    t6 r43 = d_move_ac(r42);
                    VfsDentry *r44 = r43.p1;
                    VfsDentry *r45 = r43.p2;
                    unit_t r46 = iput_ac(r26);
                    t12 r47 = (t12) {.p1 = r20, .p2 = r21};
                    SysState *r48 = ulock(r47);
                    t82 r49 = (t82) {.p1 = r48, .p2 = r13, .p3 = r45};
                    t87 r50 = (t87) {.tag = TAG_ENUM_Some, .Some = r44};
                    t84 r51 = (t84) {.tag = r50.tag, .Some = r50.Some};
                    t88 r52 = (t88) {.tag = TAG_ENUM_Success, .Success = r51};
                    t85 r53 = (t85) {.tag = r52.tag, .Success = r52.Success};
                    
                    r32 = (t86) {.p1 = r49, .p2 = r53};
                }
                r25 = r32;
            } else {
                t61 r54 = {.tag =r23.tag, .Error =r23.Error};
                t5 r55 = r54.Error;
                VfsInode *r56 = r55.p1;
                VfsDentry *r57 = r55.p2;
                unit_t r58 = dput_ac(r57);
                t12 r59 = (t12) {.p1 = r20, .p2 = r21};
                SysState *r60 = ulock(r59);
                bool_t r61;
                
                if (LETBANG_TRUE)
                    r61 = is_null_0(r56);
                else
                    ;
                
                bool_t r62 = (bool_t) {.boolean = 1U};
                bool_t r63 = (bool_t) {.boolean = r61.boolean == r62.boolean};
                t86 r64;
                
                if (r63.boolean) {
                    u64 r65;
                    
                    if (LETBANG_TRUE)
                        r65 = get_version(r13);
                    else
                        ;
                    
                    t7 r66 = (t7) {.p1 = r4, .p2 = r65};
                    VfsDentry *r67 = set_d_time(r66);
                    t5 r68 = (t5) {.p1 = r56, .p2 = r67};
                    VfsDentry *r69 = d_splice_alias_ac(r68);
                    t82 r70 = (t82) {.p1 = r60, .p2 = r13, .p3 = r69};
                    unit_t r71 = (unit_t) {.dummy = 0};
                    t89 r72 = (t89) {.tag = TAG_ENUM_None, .None = r71};
                    t84 r73 = (t84) {.tag = r72.tag, .None = r72.None};
                    t88 r74 = (t88) {.tag = TAG_ENUM_Success, .Success = r73};
                    t85 r75 = (t85) {.tag = r74.tag, .Success = r74.Success};
                    
                    r64 = (t86) {.p1 = r70, .p2 = r75};
                } else {
                    bool_t r76 = r61;
                    t5 r77 = (t5) {.p1 = r56, .p2 = r4};
                    VfsDentry *r78 = d_splice_alias_ac(r77);
                    t82 r79 = (t82) {.p1 = r60, .p2 = r13, .p3 = r78};
                    unit_t r80 = (unit_t) {.dummy = 0};
                    t89 r81 = (t89) {.tag = TAG_ENUM_None, .None = r80};
                    t84 r82 = (t84) {.tag = r81.tag, .None = r81.None};
                    t88 r83 = (t88) {.tag = TAG_ENUM_Success, .Success = r82};
                    t85 r84 = (t85) {.tag = r83.tag, .Success = r83.Success};
                    
                    r64 = (t86) {.p1 = r79, .p2 = r84};
                }
                r25 = r64;
            }
            r22 = r25;
        } else {
            t70 r85 = {.tag =r19.tag, .Error =r19.Error};
            ErrPtr *r86 = r85.Error;
            t12 r87 = (t12) {.p1 = r20, .p2 = r21};
            SysState *r88 = ulock(r87);
            t82 r89 = (t82) {.p1 = r88, .p2 = r13, .p3 = r4};
            t70 r90 = (t70) {.tag = TAG_ENUM_Error, .Error = r86};
            t85 r91 = (t85) {.tag = r90.tag, .Error = r90.Error};
            
            r22 = (t86) {.p1 = r89, .p2 = r91};
        }
        r15 = r22;
    } else {
        t70 r92 = {.tag =r14.tag, .Error =r14.Error};
        ErrPtr *r93 = r92.Error;
        t21 r94 = is_noent(r93);
        t86 r95;
        
        if (r94.tag == TAG_ENUM_Success) {
            unit_t r96 = r94.Success;
            t12 r97 = (t12) {.p1 = r8, .p2 = r9};
            SysState *r98 = ulock(r97);
            u64 r99;
            
            if (LETBANG_TRUE)
                r99 = get_version(r13);
            else
                ;
            
            t7 r100 = (t7) {.p1 = r4, .p2 = r99};
            VfsDentry *r101 = set_d_time(r100);
            unit_t r102 = (unit_t) {.dummy = 0};
            VfsInode *r103 = get_null_inode_ac(r102);
            t5 r104 = (t5) {.p1 = r103, .p2 = r101};
            VfsDentry *r105 = d_splice_alias_ac(r104);
            t82 r106 = (t82) {.p1 = r98, .p2 = r13, .p3 = r105};
            unit_t r107 = (unit_t) {.dummy = 0};
            t89 r108 = (t89) {.tag = TAG_ENUM_None, .None = r107};
            t84 r109 = (t84) {.tag = r108.tag, .None = r108.None};
            t88 r110 = (t88) {.tag = TAG_ENUM_Success, .Success = r109};
            t88 r111 = (t88) {.tag = r110.tag, .Success = r110.Success};
            t85 r112 = (t85) {.tag = r111.tag, .Success = r111.Success};
            
            r95 = (t86) {.p1 = r106, .p2 = r112};
        } else {
            t70 r113 = {.tag =r94.tag, .Error =r94.Error};
            ErrPtr *r114 = r113.Error;
            t12 r115 = (t12) {.p1 = r8, .p2 = r9};
            SysState *r116 = ulock(r115);
            t82 r117 = (t82) {.p1 = r116, .p2 = r13, .p3 = r4};
            t70 r118 = (t70) {.tag = TAG_ENUM_Error, .Error = r114};
            t85 r119 = (t85) {.tag = r118.tag, .Error = r118.Error};
            
            r95 = (t86) {.p1 = r117, .p2 = r119};
        }
        r15 = r95;
    }
    
    t86 r120 = r15;
    
    return r120;
}
t83 vfat_rmdir_cog(t82 a1)
{
    SysState *r2 = a1.p1;
    VfsInode *r3 = a1.p2;
    VfsDentry *r4 = a1.p3;
    VfsInode *r5;
    
    if (LETBANG_TRUE)
        r5 = d_inode_ac(r4);
    else
        ;
    
    Superblock *r6;
    
    if (LETBANG_TRUE)
        r6 = get_sb(r3);
    else
        ;
    
    u64 r7;
    
    if (LETBANG_TRUE)
        r7 = get_name(r4);
    else
        ;
    
    t12 r8 = (t12) {.p1 = r2, .p2 = r6};
    t12 r9 = slock(r8);
    SysState *r10 = r9.p1;
    Superblock *r11 = r9.p2;
    t21 r12 = fat_dir_empty_ac(r5);
    t83 r13;
    
    if (r12.tag == TAG_ENUM_Success) {
        unit_t r14 = r12.Success;
        t4 r15 = (t4) {.p1 = r3, .p2 = r7};
        t18 r16 = vfat_find_cog(r15);
        VfsInode *r17 = r16.p1;
        t17 r18 = r16.p2;
        t83 r19;
        
        if (r18.tag == TAG_ENUM_Success) {
            t74 r20 = (t74) {.p1 = r10, .p2 = r17, .p3 = r18.Success};
            t75 r21 = fat_remove_entries_cog(r20);
            t59 r22 = r21.p1;
            t21 r23 = r21.p2;
            SysState *r24 = r22.p1;
            VfsInode *r25 = r22.p2;
            t83 r26;
            
            if (r23.tag == TAG_ENUM_Success) {
                unit_t r27 = r23.Success;
                u64 r28;
                
                if (LETBANG_TRUE)
                    r28 = get_version(r25);
                else
                    ;
                
                VfsInode *r29 = drop_nlink_ac(r25);
                VfsInode *r30;
                
                if (LETBANG_TRUE)
                    r30 = d_inode_ac(r4);
                else
                    ;
                
                VfsInode *r31 = clear_nlink_ac(r30);
                VfsInode *r32 = update_am_time(r31);
                t59 r33 = (t59) {.p1 = r24, .p2 = r32};
                t59 r34 = fat_detach_cog(r33);
                SysState *r35 = r34.p1;
                VfsInode *r36 = r34.p2;
                t7 r37 = (t7) {.p1 = r4, .p2 = r28};
                VfsDentry *r38 = set_d_time(r37);
                t12 r39 = (t12) {.p1 = r35, .p2 = r11};
                SysState *r40 = ulock(r39);
                t82 r41 = (t82) {.p1 = r40, .p2 = r29, .p3 = r38};
                t69 r42 = (t69) {.tag = TAG_ENUM_Success, .Success = r36};
                t26 r43 = (t26) {.tag = r42.tag, .Success = r42.Success};
                
                r26 = (t83) {.p1 = r41, .p2 = r43};
            } else {
                t70 r44 = {.tag =r23.tag, .Error =r23.Error};
                ErrPtr *r45 = r44.Error;
                t12 r46 = (t12) {.p1 = r24, .p2 = r11};
                SysState *r47 = ulock(r46);
                t82 r48 = (t82) {.p1 = r47, .p2 = r25, .p3 = r4};
                t70 r49 = (t70) {.tag = TAG_ENUM_Error, .Error = r45};
                t26 r50 = (t26) {.tag = r49.tag, .Error = r49.Error};
                
                r26 = (t83) {.p1 = r48, .p2 = r50};
            }
            r19 = r26;
        } else {
            t70 r51 = {.tag =r18.tag, .Error =r18.Error};
            ErrPtr *r52 = r51.Error;
            t12 r53 = (t12) {.p1 = r10, .p2 = r11};
            SysState *r54 = ulock(r53);
            t82 r55 = (t82) {.p1 = r54, .p2 = r17, .p3 = r4};
            t70 r56 = (t70) {.tag = TAG_ENUM_Error, .Error = r52};
            t26 r57 = (t26) {.tag = r56.tag, .Error = r56.Error};
            
            r19 = (t83) {.p1 = r55, .p2 = r57};
        }
        r13 = r19;
    } else {
        t70 r58 = {.tag =r12.tag, .Error =r12.Error};
        ErrPtr *r59 = r58.Error;
        t12 r60 = (t12) {.p1 = r10, .p2 = r11};
        SysState *r61 = ulock(r60);
        t82 r62 = (t82) {.p1 = r61, .p2 = r3, .p3 = r4};
        t70 r63 = (t70) {.tag = TAG_ENUM_Error, .Error = r59};
        t26 r64 = (t26) {.tag = r63.tag, .Error = r63.Error};
        
        r13 = (t83) {.p1 = r62, .p2 = r64};
    }
    
    t83 r65 = r13;
    
    return r65;
}
t83 vfat_unlink_cog(t82 a1)
{
    SysState *r2 = a1.p1;
    VfsInode *r3 = a1.p2;
    VfsDentry *r4 = a1.p3;
    Superblock *r5;
    
    if (LETBANG_TRUE)
        r5 = get_sb(r3);
    else
        ;
    
    u64 r6;
    
    if (LETBANG_TRUE)
        r6 = get_version(r3);
    else
        ;
    
    t12 r7 = (t12) {.p1 = r2, .p2 = r5};
    t12 r8 = slock(r7);
    SysState *r9 = r8.p1;
    Superblock *r10 = r8.p2;
    u64 r11;
    
    if (LETBANG_TRUE)
        r11 = get_name(r4);
    else
        ;
    
    t4 r12 = (t4) {.p1 = r3, .p2 = r11};
    t18 r13 = vfat_find_cog(r12);
    VfsInode *r14 = r13.p1;
    t17 r15 = r13.p2;
    t83 r16;
    
    if (r15.tag == TAG_ENUM_Success) {
        t74 r17 = (t74) {.p1 = r9, .p2 = r14, .p3 = r15.Success};
        t75 r18 = fat_remove_entries_cog(r17);
        t59 r19 = r18.p1;
        t21 r20 = r18.p2;
        SysState *r21 = r19.p1;
        VfsInode *r22 = r19.p2;
        t83 r23;
        
        if (r20.tag == TAG_ENUM_Success) {
            unit_t r24 = r20.Success;
            VfsInode *r25;
            
            if (LETBANG_TRUE)
                r25 = d_inode_ac(r4);
            else
                ;
            
            VfsInode *r26 = clear_nlink_ac(r25);
            VfsInode *r27 = update_am_time(r26);
            t59 r28 = (t59) {.p1 = r21, .p2 = r27};
            t59 r29 = fat_detach_cog(r28);
            SysState *r30 = r29.p1;
            VfsInode *r31 = r29.p2;
            t7 r32 = (t7) {.p1 = r4, .p2 = r6};
            VfsDentry *r33 = set_d_time(r32);
            t12 r34 = (t12) {.p1 = r30, .p2 = r10};
            SysState *r35 = ulock(r34);
            t82 r36 = (t82) {.p1 = r35, .p2 = r22, .p3 = r33};
            t69 r37 = (t69) {.tag = TAG_ENUM_Success, .Success = r31};
            t26 r38 = (t26) {.tag = r37.tag, .Success = r37.Success};
            
            r23 = (t83) {.p1 = r36, .p2 = r38};
        } else {
            t70 r39 = {.tag =r20.tag, .Error =r20.Error};
            ErrPtr *r40 = r39.Error;
            t12 r41 = (t12) {.p1 = r21, .p2 = r10};
            SysState *r42 = ulock(r41);
            t82 r43 = (t82) {.p1 = r42, .p2 = r22, .p3 = r4};
            t70 r44 = (t70) {.tag = TAG_ENUM_Error, .Error = r40};
            t26 r45 = (t26) {.tag = r44.tag, .Error = r44.Error};
            
            r23 = (t83) {.p1 = r43, .p2 = r45};
        }
        r16 = r23;
    } else {
        t70 r46 = {.tag =r15.tag, .Error =r15.Error};
        ErrPtr *r47 = r46.Error;
        t12 r48 = (t12) {.p1 = r9, .p2 = r10};
        SysState *r49 = ulock(r48);
        t82 r50 = (t82) {.p1 = r49, .p2 = r14, .p3 = r4};
        t70 r51 = (t70) {.tag = TAG_ENUM_Error, .Error = r47};
        t26 r52 = (t26) {.tag = r51.tag, .Error = r51.Error};
        
        r16 = (t83) {.p1 = r50, .p2 = r52};
    }
    
    t83 r53 = r16;
    
    return r53;
}
t56 iterate_0(t54 args)
{
    t46 generator_args = {.acc =args.acc, .obsv =args.obsv};
    t50 consumer_args = {.obsv =args.obsv};
    t56 ret;
    t48 generator_ret;
    t52 consumer_ret;
    
    while (1) {
        generator_ret = dispatch_t49(args.gen, generator_args);
        ret.p1 = generator_ret.p1;
        if (generator_ret.p2.tag == TAG_ENUM_Stop) {
            ret.p2.tag = TAG_ENUM_Stop;
            ret.p2.Stop = generator_ret.p2.Stop;
            return ret;
        } else if (generator_ret.p2.tag == TAG_ENUM_Return) {
            ret.p2.tag = TAG_ENUM_Return;
            ret.p2.Return = generator_ret.p2.Return;
            return ret;
        }
        consumer_args.acc = ret.p1;
        consumer_args.obj = generator_ret.p2.Yield;
        consumer_ret = dispatch_t53(args.cons, consumer_args);
        ret.p1 = consumer_ret.p1;
        if (consumer_ret.p2.tag == TAG_ENUM_Stop) {
            ret.p2.tag = TAG_ENUM_Stop;
            ret.p2.Stop = consumer_ret.p2.Stop;
            return ret;
        } else if (consumer_ret.p2.tag == TAG_ENUM_Return) {
            ret.p2.tag = TAG_ENUM_Return;
            ret.p2.Return = consumer_ret.p2.Return;
            return ret;
        }
        generator_args.acc = ret.p1;
    }
}
t45 iterate_1(t43 args)
{
    t34 generator_args = {.acc =args.acc, .obsv =args.obsv};
    t39 consumer_args = {.obsv =args.obsv};
    t45 ret;
    t37 generator_ret;
    t41 consumer_ret;
    
    while (1) {
        generator_ret = dispatch_t38(args.gen, generator_args);
        ret.p1 = generator_ret.p1;
        if (generator_ret.p2.tag == TAG_ENUM_Stop) {
            ret.p2.tag = TAG_ENUM_Stop;
            ret.p2.Stop = generator_ret.p2.Stop;
            return ret;
        } else if (generator_ret.p2.tag == TAG_ENUM_Return) {
            ret.p2.tag = TAG_ENUM_Return;
            ret.p2.Return = generator_ret.p2.Return;
            return ret;
        }
        consumer_args.acc = ret.p1;
        consumer_args.obj = generator_ret.p2.Yield;
        consumer_ret = dispatch_t42(args.cons, consumer_args);
        ret.p1 = consumer_ret.p1;
        if (consumer_ret.p2.tag == TAG_ENUM_Stop) {
            ret.p2.tag = TAG_ENUM_Stop;
            ret.p2.Stop = consumer_ret.p2.Stop;
            return ret;
        } else if (consumer_ret.p2.tag == TAG_ENUM_Return) {
            ret.p2.tag = TAG_ENUM_Return;
            ret.p2.Return = consumer_ret.p2.Return;
            return ret;
        }
        generator_args.acc = ret.p1;
    }
}
static inline void fat_lock_build_inode(struct msdos_sb_info *sbi)
{
    if (sbi->options.nfs == FAT_NFS_NOSTALE_RO)
        mutex_lock(&sbi->nfs_build_inode_lock);
}
static inline void fat_unlock_build_inode(struct msdos_sb_info *sbi)
{
    if (sbi->options.nfs == FAT_NFS_NOSTALE_RO)
        mutex_unlock(&sbi->nfs_build_inode_lock);
}
struct timespec *cog_to_c_time(t8 arg)
{
    struct timespec *ts = kmalloc(sizeof(struct timespec), GFP_KERNEL);
    
    if (!ts)
        printk("ERROR MALLOCING.\n");
    ts->tv_sec = arg.seconds;
    ts->tv_nsec = arg.nanoseconds;
    return ts;
}
u64 get_version(VfsInode *arg)
{
    struct inode *c_inode = (struct inode *) arg;
    
    return c_inode->i_version;
}
Superblock *get_sb(VfsInode *arg)
{
    struct inode *c_inode = (struct inode *) arg;
    Superblock *ret = (Superblock *) c_inode->i_sb;
    
    return ret;
}
u64 get_b_data(u64 arg)
{
    struct buffer_head *bh = (struct buffer_head *) arg;
    struct msdos_dir_entry *bdat = (struct msdos_dir_entry *) bh->b_data;
    
    return (u64) bdat;
}
u64 get_name(VfsDentry *dentry)
{
    struct dentry *c_dentry = (struct dentry *) dentry;
    u64 ret = (u64) &c_dentry->d_name;
    
    return ret;
}
VfsDentry *get_parent(VfsDentry *arg)
{
    struct dentry *dentry = (struct dentry *) arg;
    struct dentry *parent = dentry->d_parent;
    
    return (VfsDentry *) parent;
}
u64 get_head(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    struct msdos_sb_info *sbi = MSDOS_SB(inode->i_sb);
    struct hlist_head *head = sbi->inode_hashtable;
    
    return (u64) head;
}
u64 get_dhead(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    struct msdos_sb_info *sbi = MSDOS_SB(inode->i_sb);
    
    return (u64) sbi->dir_hashtable;
}
u64 get_endpoint_ac(t24 arg)
{
    struct super_block *sb = (struct super_block *) arg.p1;
    struct buffer_head *bh = (struct buffer_head *) arg.p2;
    struct msdos_dir_entry *endp = (struct msdos_dir_entry *) (bh->b_data +
                                                               sb->s_blocksize);
    
    return (u64) endp;
}
t10 get_current_time(SysState *ex)
{
    struct timespec ts = CURRENT_TIME_SEC;
    t10 ret;
    
    ret.p1 = ex;
    ret.p2.seconds = ts.tv_sec;
    ret.p2.nanoseconds = ts.tv_nsec;
    return ret;
}
t3 get_logstart(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    struct msdos_inode_info *minfo = MSDOS_I(inode);
    int logstart = minfo->i_logstart;
    t3 ret;
    
    ret.p1 = (VfsInode *) inode;
    ret.p2 = logstart;
    return ret;
}
char *get_qstr_name(u64 arg)
{
    struct qstr *qname = (struct qstr *) arg;
    const char *name = qname->name;
    
    return (char *) name;
}
u32 get_qstr_length(u64 arg)
{
    struct qstr *qname = (struct qstr *) arg;
    unsigned int len = qname->len;
    
    return len;
}
VfsInode *get_null_inode_ac(unit_t arg)
{
    VfsInode *ret = 0;
    
    return ret;
}
u64 get_fat_hash(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    struct msdos_inode_info *minfo = MSDOS_I(inode);
    struct hlist_node *node_fat_hash = &minfo->i_fat_hash;
    
    return (u64) node_fat_hash;
}
u64 get_dir_hash(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    struct msdos_inode_info *minfo = MSDOS_I(inode);
    struct hlist_node *node_dir_hash = &minfo->i_dir_hash;
    
    return (u64) node_dir_hash;
}
VfsInode *set_version(t4 arg)
{
    struct inode *c_inode = (struct inode *) arg.p1;
    
    c_inode->i_version = (u64) arg.p2;
    
    VfsInode *ret = (VfsInode *) c_inode;
    
    return ret;
}
VfsInode *set_time(t9 arg)
{
    struct inode *node = (struct inode *) arg.p1;
    t8 ts_arg = arg.p2;
    struct timespec ts;
    
    ts.tv_sec = ts_arg.seconds;
    ts.tv_nsec = ts_arg.nanoseconds;
    node->i_mtime = node->i_atime = node->i_ctime = ts;
    
    VfsInode *ret = (VfsInode *) node;
    
    return ret;
}
VfsInode *set_ipos(t4 arg)
{
    struct inode *inode = (struct inode *) arg.p1;
    loff_t i_pos = arg.p2;
    
    MSDOS_I(inode)->i_pos = i_pos;
    return (VfsInode *) inode;
}
t13 set_ino(t13 arg)
{
    struct inode *inode = (struct inode *) arg.p1;
    struct super_block *sb = (struct super_block *) arg.p2;
    
    inode->i_ino = iunique(sb, MSDOS_ROOT_INO);
    
    t13 ret;
    
    ret.p1 = (VfsInode *) inode;
    ret.p2 = (Superblock *) sb;
    return ret;
}
VfsDentry *set_d_time(t7 arg)
{
    struct dentry *dentry = (struct dentry *) arg.p1;
    
    dentry->d_time = arg.p2;
    return (VfsDentry *) dentry;
}
bool_t ptr_grq_ac(t1 arg)
{
    bool_t ret;
    void *first_ptr = (void *) arg.p1;
    void *second_ptr = (void *) arg.p2;
    
    if (first_ptr > second_ptr)
        ret.boolean = true;
    else
        ret.boolean = false;
    return ret;
}
bool_t ptr_greq_ac(t1 arg)
{
    bool_t ret;
    void *first_ptr = (void *) arg.p1;
    void *second_ptr = (void *) arg.p2;
    
    if (first_ptr >= second_ptr)
        ret.boolean = true;
    else
        ret.boolean = false;
    return ret;
}
t26 is_err(VfsInode *inode)
{
    t26 ret;
    struct inode *c_inode = (struct inode *) inode;
    
    if (IS_ERR(c_inode)) {
        ret.tag = TAG_ENUM_Error;
        ret.Error = (ErrPtr *) inode;
        return ret;
    } else {
        ret.tag = TAG_ENUM_Success;
        ret.Success = inode;
        return ret;
    }
}
bool_t is_root_den(VfsDentry *arg)
{
    struct dentry *dentry = (struct dentry *) arg;
    bool_t ret;
    
    if (IS_ROOT(dentry))
        ret.boolean = true;
    else
        ret.boolean = false;
    return ret;
}
bool_t is_root_ino(VfsInode *arg)
{
    bool_t ret;
    struct inode *inode = (struct inode *) arg;
    
    if (inode->i_ino != MSDOS_ROOT_INO) {
        ret.boolean = false;
        return ret;
    } else {
        ret.boolean = true;
        return ret;
    }
}
bool_t is_nfs(VfsInode *arg)
{
    bool_t ret;
    struct inode *inode = (struct inode *) arg;
    struct msdos_sb_info *sbi = MSDOS_SB(inode->i_sb);
    
    if (sbi->options.nfs) {
        ret.boolean = true;
        return ret;
    } else {
        ret.boolean = false;
        return ret;
    }
}
bool_t is_dir_ac(VfsInode *arg)
{
    bool_t ret;
    struct inode *inode = (struct inode *) arg;
    
    if (S_ISDIR(inode->i_mode)) {
        ret.boolean = true;
        return ret;
    } else {
        ret.boolean = false;
        return ret;
    }
}
bool_t is_null_1(VfsDentry *arg)
{
    bool_t ret;
    
    if (!arg) {
        ret.boolean = true;
        return ret;
    } else {
        ret.boolean = false;
        return ret;
    }
}
bool_t is_null_0(VfsInode *arg)
{
    bool_t ret;
    
    if (!arg) {
        ret.boolean = true;
        return ret;
    } else {
        ret.boolean = false;
        return ret;
    }
}
t21 is_noent(ErrPtr *errptr)
{
    t21 ret;
    int err = PTR_ERR((void *) errptr);
    
    if (err == -ENOENT) {
        ret.tag = TAG_ENUM_Success;
        return ret;
    } else {
        ret.tag = TAG_ENUM_Error;
        ret.Error = errptr;
        return ret;
    }
}
bool_t is_equal_den(t6 arg)
{
    struct dentry *first_den = (struct dentry *) arg.p1;
    struct dentry *second_den = (struct dentry *) arg.p2;
    bool_t ret;
    
    if (first_den == second_den)
        ret.boolean = true;
    else
        ret.boolean = false;
    return ret;
}
bool_t is_disconnected(VfsDentry *arg)
{
    bool_t ret;
    struct dentry *dentry = (struct dentry *) arg;
    
    if (dentry->d_flags & DCACHE_DISCONNECTED)
        ret.boolean = true;
    else
        ret.boolean = false;
    return ret;
}
bool_t is_dirsync_ac(VfsInode *arg)
{
    bool_t ret;
    struct inode *inode = (struct inode *) arg;
    
    if (IS_DIRSYNC(inode))
        ret.boolean = true;
    else
        ret.boolean = false;
    return ret;
}
t12 flock_buildinode(t12 arg)
{
    struct super_block *sb = (struct super_block *) arg.p2;
    
    fat_lock_build_inode(MSDOS_SB(sb));
    
    t12 ret;
    
    ret.p1 = arg.p1;
    ret.p2 = (Superblock *) sb;
    return ret;
}
t12 spdir_lock(t12 arg)
{
    struct super_block *sb = (struct super_block *) arg.p2;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    
    spin_lock(&sbi->dir_hash_lock);
    
    t12 ret;
    
    ret.p1 = arg.p1;
    ret.p2 = (Superblock *) sb;
    return ret;
}
t12 spinode_lock(t12 arg)
{
    struct super_block *sb = (struct super_block *) arg.p2;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    
    spin_lock(&sbi->inode_hash_lock);
    
    t12 ret;
    
    ret.p1 = arg.p1;
    ret.p2 = (Superblock *) sb;
    return ret;
}
t12 slock(t12 arg)
{
    struct super_block *sb = (struct super_block *) arg.p2;
    
    mutex_lock(&MSDOS_SB(sb)->s_lock);
    
    t12 ret;
    
    ret.p1 = arg.p1;
    ret.p2 = (Superblock *) sb;
    return ret;
}
t12 funlock_buildinode(t12 arg)
{
    struct super_block *sb = (struct super_block *) arg.p2;
    
    fat_unlock_build_inode(MSDOS_SB(sb));
    
    t12 ret;
    
    ret.p1 = arg.p1;
    ret.p2 = (Superblock *) sb;
    return ret;
}
SysState *spdir_unlock(t12 arg)
{
    struct super_block *sb = (struct super_block *) arg.p2;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    
    spin_unlock(&sbi->dir_hash_lock);
    return arg.p1;
}
SysState *spinode_unlock(t12 arg)
{
    struct super_block *sb = (struct super_block *) arg.p2;
    struct msdos_sb_info *sbi = MSDOS_SB(sb);
    
    spin_unlock(&sbi->inode_hash_lock);
    return arg.p1;
}
SysState *ulock(t12 arg)
{
    Superblock *sblock = arg.p2;
    struct super_block *sb = (struct super_block *) sblock;
    
    mutex_unlock(&MSDOS_SB(sb)->s_lock);
    return arg.p1;
}
t20 fat_alloc_new_dir_ac(t9 arg)
{
    t20 ret;
    struct inode *inode = (struct inode *) arg.p1;
    struct timespec *ts = cog_to_c_time(arg.p2);
    int cluster = fat_alloc_new_dir(inode, ts);
    
    kfree(ts);
    ret.p1 = (VfsInode *) inode;
    if (cluster < 0) {
        ErrPtr *err = (ErrPtr *) ERR_PTR(cluster);
        
        ret.p2.tag = TAG_ENUM_Error;
        ret.p2.Error = err;
    } else {
        ret.p2.tag = TAG_ENUM_Success;
        ret.p2.Success = cluster;
    }
    return ret;
}
VfsInode *inc_nlink_ac(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    
    inc_nlink(inode);
    return (VfsInode *) inode;
}
VfsInode *set_nlink_ac(t3 arg)
{
    set_nlink((struct inode *) arg.p1, arg.p2);
    return arg.p1;
}
VfsInode *fat_free_clusters_ac(t3 arg)
{
    fat_free_clusters((struct inode *) arg.p1, arg.p2);
    return arg.p1;
}
t21 fat_dir_empty_ac(VfsInode *arg)
{
    t21 ret;
    struct inode *inode = (struct inode *) arg;
    int err = fat_dir_empty(inode);
    
    if (err) {
        ret.tag = TAG_ENUM_Error;
        ret.Error = (ErrPtr *) ERR_PTR(err);
        return ret;
    } else {
        ret.tag = TAG_ENUM_Success;
        return ret;
    }
}
t18 fat_search_long_ac(t14 arg)
{
    t18 ret;
    struct inode *inode = (struct inode *) arg.p1;
    struct qstr *qname = (struct qstr *) arg.p2;
    int len = arg.p3;
    struct fat_slot_info sinfo;
    int err = fat_search_long(inode, qname->name, len, &sinfo);
    
    ret.p1 = (VfsInode *) inode;
    if (err) {
        ret.p2.tag = TAG_ENUM_Error;
        
        void *err_ptr = ERR_PTR(err);
        
        ret.p2.Error = (ErrPtr *) err_ptr;
        return ret;
    } else {
        ret.p2.tag = TAG_ENUM_Success;
        ret.p2.Success.i_pos = (u64) sinfo.i_pos;
        ret.p2.Success.slot_off = (u64) sinfo.slot_off;
        ret.p2.Success.nr_slots = (u32) sinfo.nr_slots;
        ret.p2.Success.de = (u64) sinfo.de;
        ret.p2.Success.bh = (u64) sinfo.bh;
        return ret;
    }
}
int vfat_unlink_ac(struct inode *inode, struct dentry *dentry)
{
    VfsInode *inode_cog = (VfsInode *) inode;
    VfsDentry *dentry_cog = (VfsDentry *) dentry;
    t82 arg;
    
    arg.p1 = 0;
    arg.p2 = inode_cog;
    arg.p3 = dentry_cog;
    
    t83 cog_ret = dispatch_t137(FUN_ENUM_vfat_unlink_cog, arg);
    
    if (cog_ret.p2.tag == TAG_ENUM_Error)
        return PTR_ERR((void *) cog_ret.p2.Error);
    else
        return 0;
}
t4 mark_buffer_dirty_inode_ac(t4 arg)
{
    struct buffer_head *bh = (struct buffer_head *) arg.p2;
    struct inode *inode = (struct inode *) arg.p1;
    
    mark_buffer_dirty_inode(bh, inode);
    
    t4 ret;
    
    ret.p1 = (VfsInode *) inode;
    ret.p2 = (u64) bh;
    return ret;
}
t28 sync_dirty_buffer_ac(u64 arg)
{
    t28 ret;
    int err = sync_dirty_buffer((struct buffer_head *) arg);
    
    if (err) {
        ret.p1 = arg;
        ret.p2.tag = TAG_ENUM_Error;
        ret.p2.Error = (ErrPtr *) ERR_PTR(err);
        return ret;
    } else {
        ret.p1 = arg;
        ret.p2.tag = TAG_ENUM_Success;
        return ret;
    }
}
VfsInode *deep_fat_write_inode_ac(t3 arg)
{
    struct inode *inode = (struct inode *) arg.p1;
    
    __fat_write_inode(inode, arg.p2);
    return (VfsInode *) inode;
}
t15 deep_fat_remove_entries_ac(t14 arg)
{
    t15 ret;
    int err = __fat_remove_entries((struct inode *) arg.p1, arg.p2, arg.p3);
    
    if (!err) {
        ret.tag = TAG_ENUM_Success;
        ret.Success = arg.p1;
        return ret;
    } else {
        ret.tag = TAG_ENUM_Error;
        ret.Error = arg.p1;
        return ret;
    }
}
u64 delete_first_plus(u64 arg)
{
    struct msdos_dir_entry *de = (struct msdos_dir_entry *) arg;
    
    de->name[0] = DELETED_FLAG;
    de++;
    return (u64) de;
}
u64 delete_first_ac(u64 arg)
{
    struct msdos_dir_entry *de = (struct msdos_dir_entry *) arg;
    
    de->name[0] = DELETED_FLAG;
    de--;
    return (u64) de;
}
SysState *fat_msg_ac(t11 arg)
{
    fat_msg((struct super_block *) arg.p2, KERN_WARNING, arg.p3);
    
    SysState *ex = arg.p1;
    
    return ex;
}
VfsInode *drop_nlink_ac(VfsInode *arg)
{
    drop_nlink((struct inode *) arg);
    return (VfsInode *) arg;
}
VfsInode *d_inode_ac(VfsDentry *arg)
{
    struct inode *inode = d_inode((struct dentry *) arg);
    
    return (VfsInode *) inode;
}
unit_t fat_detach_ac(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    
    fat_detach(inode);
    
    unit_t ret;
    
    return ret;
}
VfsInode *clear_nlink_ac(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    
    clear_nlink(inode);
    return (VfsInode *) inode;
}
u64 hash_32_ac_0(u32 arg)
{
    return hash_32((u32) arg, FAT_HASH_BITS);
}
u64 hash_32_ac_1(u64 arg)
{
    return hash_32((u32) arg, FAT_HASH_BITS);
}
VfsInode *hlist_add_head_ac(t4 arg)
{
    struct inode *inode = (struct inode *) arg.p1;
    struct msdos_inode_info *minfo = MSDOS_I(inode);
    struct hlist_node *node = &minfo->i_fat_hash;
    struct hlist_head *head = (struct hlist_head *) arg.p2;
    
    hlist_add_head(node, head);
    return (VfsInode *) inode;
}
VfsInode *update_am_time(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    
    inode->i_mtime = inode->i_atime = CURRENT_TIME_SEC;
    return (VfsInode *) inode;
}
VfsDentry *d_splice_alias_ac(t5 arg)
{
    struct inode *inode = (struct inode *) arg.p1;
    struct dentry *dentry = (struct dentry *) arg.p2;
    struct dentry *ret = d_splice_alias(inode, dentry);
    
    return (VfsDentry *) ret;
}
VfsDentry *d_unhashed_ac(VfsDentry *arg)
{
    struct dentry *alias = (struct dentry *) arg;
    
    BUG_ON(d_unhashed(alias));
    return (VfsDentry *) alias;
}
unit_t hlist_del_init_ac(u64 arg)
{
    hlist_del_init((struct hlist_node *) arg);
    
    unit_t ret;
    
    return ret;
}
t5 d_find_alias_ac(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    struct dentry *alias = d_find_alias(inode);
    t5 ret;
    
    ret.p1 = (VfsInode *) inode;
    ret.p2 = (VfsDentry *) alias;
    return ret;
}
unit_t dput_ac(VfsDentry *arg)
{
    struct dentry *dentry = (struct dentry *) arg;
    
    dput(dentry);
    
    unit_t ret;
    
    return ret;
}
t6 d_move_ac(t6 arg)
{
    struct dentry *alias = (struct dentry *) arg.p1;
    struct dentry *dentry = (struct dentry *) arg.p2;
    t6 ret;
    
    d_move(alias, dentry);
    ret.p1 = (VfsDentry *) alias;
    ret.p2 = (VfsDentry *) dentry;
    return ret;
}
VfsInode *mark_inode_dirty_ac(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    
    mark_inode_dirty(inode);
    return (VfsInode *) inode;
}
VfsInode *fat_sync_inode_ac(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    
    (void) fat_sync_inode(inode);
    return (VfsInode *) inode;
}
bool_t last_char_dot_ac(t2 arg)
{
    bool_t ret;
    char *name = (char *) arg.p1;
    u32 len = arg.p2;
    
    if (name[len - 1] == '.')
        ret.boolean = true;
    else
        ret.boolean = false;
    return ret;
}
ErrPtr *noent_ac(unit_t arg)
{
    ErrPtr *ret;
    
    ret = (ErrPtr *) ERR_PTR(-ENOENT);
    return ret;
}
u32 decrementU32(u32 arg)
{
    return arg - 1;
}
t18 fat_add_entries_ac(t14 arg)
{
    t18 ret;
    struct inode *inode = (struct inode *) arg.p1;
    struct msdos_dir_slot *slots = (struct msdos_dir_slot *) arg.p2;
    int nr_slots = arg.p3;
    struct fat_slot_info sinfo;
    int err = fat_add_entries(inode, slots, nr_slots, &sinfo);
    
    kfree(slots);
    ret.p1 = (VfsInode *) inode;
    if (err) {
        ret.p2.tag = TAG_ENUM_Error;
        ret.p2.Error = (ErrPtr *) ERR_PTR(err);
        return ret;
    } else {
        ret.p2.tag = TAG_ENUM_Success;
        ret.p2.Success.i_pos = (u64) sinfo.i_pos;
        ret.p2.Success.slot_off = (u64) sinfo.slot_off;
        ret.p2.Success.nr_slots = (u32) sinfo.nr_slots;
        ret.p2.Success.de = (u64) sinfo.de;
        ret.p2.Success.bh = (u64) sinfo.bh;
        return ret;
    }
}
t32 vfat_build_slots_ac(t29 arg)
{
    t32 ret;
    struct inode *inode = (struct inode *) arg.p1;
    const char *name = ((struct qstr *) arg.p2)->name;
    int len = arg.p3;
    int is_dir = arg.p4;
    int cluster = arg.p5;
    struct timespec *ts = cog_to_c_time(arg.p6);
    int nr_slots;
    struct msdos_dir_slot *slots;
    
    ret.p1 = (VfsInode *) inode;
    slots = kmalloc(sizeof(*slots) * MSDOS_SLOTS, GFP_NOFS);
    if (slots == NULL) {
        ret.p2.tag = TAG_ENUM_Error;
        ret.p2.Error = (ErrPtr *) ERR_PTR(-ENOMEM);
        return ret;
    }
    
    int err = vfat_build_slots(inode, name, len, is_dir, cluster, ts, slots,
                               &nr_slots);
    
    kfree(ts);
    if (err) {
        kfree(slots);
        ret.p2.tag = TAG_ENUM_Error;
        ret.p2.Error = (ErrPtr *) ERR_PTR(err);
        return ret;
    } else {
        ret.p2.tag = TAG_ENUM_Success;
        ret.p2.Success.p1 = (u64) slots;
        ret.p2.Success.p2 = nr_slots;
        return ret;
    }
}
t27 new_inode_ac(Superblock *arg)
{
    t27 ret;
    struct super_block *sb = (struct super_block *) arg;
    struct inode *newinode = new_inode(sb);
    
    ret.p1 = (Superblock *) sb;
    if (!newinode) {
        ret.p2.tag = TAG_ENUM_Error;
        ret.p2.Error = (ErrPtr *) ERR_PTR(-ENOMEM);
        return ret;
    } else {
        ret.p2.tag = TAG_ENUM_Success;
        ret.p2.Success = (VfsInode *) newinode;
        return ret;
    }
}
t25 fat_iget_ac(t24 arg)
{
    t25 ret;
    struct super_block *sb = (struct super_block *) arg.p1;
    loff_t i_pos = arg.p2;
    struct inode *inode = fat_iget(sb, i_pos);
    
    ret.p1 = (Superblock *) sb;
    if (inode) {
        ret.p2.tag = TAG_ENUM_Error;
        ret.p2.Error = (ErrPtr *) inode;
        return ret;
    } else {
        ret.p2.tag = TAG_ENUM_Success;
        return ret;
    }
}
t23 fat_fill_inode_ac(t4 arg)
{
    t23 ret;
    struct inode *inode = (struct inode *) arg.p1;
    struct msdos_dir_entry *de = (struct msdos_dir_entry *) arg.p2;
    int err = fat_fill_inode(inode, de);
    
    if (err) {
        ret.tag = TAG_ENUM_Error;
        ret.Error.p1 = (VfsInode *) inode;
        ret.Error.p2 = (ErrPtr *) ERR_PTR(err);
        return ret;
    } else {
        ret.tag = TAG_ENUM_Success;
        ret.Success = (VfsInode *) inode;
        return ret;
    }
}
VfsInode *insert_inode_hash_ac(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    
    insert_inode_hash(inode);
    return (VfsInode *) inode;
}
unit_t iput_ac(VfsInode *arg)
{
    struct inode *inode = (struct inode *) arg;
    
    iput(inode);
    
    unit_t ret;
    
    return ret;
}
u32 ptr_err_ac(ErrPtr *arg)
{
    void *err_ptr = (void *) arg;
    
    return PTR_ERR(err_ptr);
}
int vfat_create_ac(struct inode *dir, struct dentry *dentry, umode_t mode,
                   bool excl)
{
    VfsInode *cog_inode = (VfsInode *) dir;
    VfsDentry *cog_dentry = (VfsDentry *) dentry;
    t82 arg;
    
    arg.p1 = 0;
    arg.p2 = cog_inode;
    arg.p3 = cog_dentry;
    
    t83 ret = dispatch_t137(FUN_ENUM_vfat_create_cog, arg);
    
    if (ret.p2.tag == TAG_ENUM_Success)
        return 0;
    else
        return PTR_ERR((void *) ret.p2.Error);
}
u64 brelse_ac(u64 arg)
{
    struct buffer_head *bh = (struct buffer_head *) arg;
    
    brelse(bh);
    return arg;
}
t5 d_instantiate_ac(t5 arg)
{
    struct inode *inode = (struct inode *) arg.p1;
    struct dentry *dentry = (struct dentry *) arg.p2;
    
    d_instantiate(dentry, inode);
    
    t5 ret;
    
    ret.p1 = (VfsInode *) inode;
    ret.p2 = (VfsDentry *) dentry;
    return ret;
}
struct dentry *vfat_lookup_ac(struct inode *dir, struct dentry *dentry,
                              unsigned int flags)
{
    VfsInode *cog_dir = (VfsInode *) dir;
    VfsDentry *cog_dentry = (VfsDentry *) dentry;
    t82 arg;
    
    arg.p1 = 0;
    arg.p2 = cog_dir;
    arg.p3 = cog_dentry;
    
    t86 ret = dispatch_t138(FUN_ENUM_vfat_lookup_cog, arg);
    t85 result = ret.p2;
    
    if (result.tag == TAG_ENUM_Success) {
        if (result.Success.tag == TAG_ENUM_Some)
            return (struct dentry *) result.Success.Some;
        else
            return (struct dentry *) ret.p1.p3;
    } else
        return (struct dentry *) result.Error;
}
u64 incr_head_pointer(t1 arg)
{
    struct hlist_head *ptr = (struct hlist_head *) arg.p1;
    
    return (u64) (ptr + arg.p2);
}
int vfat_mkdir_ac(struct inode *inode, struct dentry *dentry, umode_t mode)
{
    t82 arg;
    
    arg.p1 = 0;
    arg.p2 = (VfsInode *) inode;
    arg.p3 = (VfsDentry *) dentry;
    
    t83 cog_ret = dispatch_t137(FUN_ENUM_vfat_mkdir_cog, arg);
    
    if (cog_ret.p2.tag == TAG_ENUM_Success)
        return 0;
    else
        return PTR_ERR((void *) cog_ret.p2.Error);
}
int vfat_rmdir_ac(struct inode *dir, struct dentry *dentry)
{
    t82 arg;
    
    arg.p1 = (SysState *) 0;
    arg.p2 = (VfsInode *) dir;
    arg.p3 = (VfsDentry *) dentry;
    
    t83 cog_ret = dispatch_t137(FUN_ENUM_vfat_rmdir_cog, arg);
    
    if (cog_ret.p2.tag == TAG_ENUM_Success)
        return 0;
    else
        return PTR_ERR((void *) cog_ret.p2.Error);
}


