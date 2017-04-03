// This build info header is now disabled by --fno-gen-header.
// --------------------------------------------------------------------------------
// We strongly discourage users from generating individual files for Isabelle
// proofs, as it will end up with an inconsistent collection of output files (i.e.
// Isabelle input files).

#ifndef GENERATED_H__
#define GENERATED_H__

#include <cogent-defns.h>

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
#define FUN_DISP_MACRO_dispatch_t90(a1, a2, a3)\
{\
    {\
        a1 = is_noent(a3);\
    }\
}
typedef untyped_func_enum t91;
#define FUN_DISP_MACRO_dispatch_t91(a1, a2, a3)\
{\
    {\
        a1 = new_inode_ac(a3);\
    }\
}
typedef untyped_func_enum t92;
#define FUN_DISP_MACRO_dispatch_t92(a1, a2, a3)\
{\
    {\
        a1 = get_current_time(a3);\
    }\
}
typedef untyped_func_enum t93;
#define FUN_DISP_MACRO_dispatch_t93(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_d_unhashed_ac:\
            {\
                a1 = d_unhashed_ac(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = get_parent(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t94;
#define FUN_DISP_MACRO_dispatch_t94(a1, a2, a3)\
{\
    {\
        a1 = d_inode_ac(a3);\
    }\
}
typedef untyped_func_enum t95;
#define FUN_DISP_MACRO_dispatch_t95(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_is_disconnected:\
            {\
                a1 = is_disconnected(a3);\
                break;\
            }\
            \
          case FUN_ENUM_is_null_1:\
            {\
                a1 = is_null_1(a3);\
                break;\
            }\
            \
          case FUN_ENUM_is_root_den:\
            {\
                a1 = is_root_den(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = vfat_d_anon_disconn_cog(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t96;
#define FUN_DISP_MACRO_dispatch_t96(a1, a2, a3)\
{\
    {\
        a1 = get_name(a3);\
    }\
}
typedef untyped_func_enum t97;
#define FUN_DISP_MACRO_dispatch_t97(a1, a2, a3)\
{\
    {\
        a1 = dput_ac(a3);\
    }\
}
typedef untyped_func_enum t98;
#define FUN_DISP_MACRO_dispatch_t98(a1, a2, a3)\
{\
    {\
        a1 = get_sb(a3);\
    }\
}
typedef untyped_func_enum t99;
#define FUN_DISP_MACRO_dispatch_t99(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_clear_nlink_ac:\
            {\
                a1 = clear_nlink_ac(a3);\
                break;\
            }\
            \
          case FUN_ENUM_drop_nlink_ac:\
            {\
                a1 = drop_nlink_ac(a3);\
                break;\
            }\
            \
          case FUN_ENUM_fat_sync_inode_cog:\
            {\
                a1 = fat_sync_inode_cog(a3);\
                break;\
            }\
            \
          case FUN_ENUM_inc_nlink_ac:\
            {\
                a1 = inc_nlink_ac(a3);\
                break;\
            }\
            \
          case FUN_ENUM_incr_version:\
            {\
                a1 = incr_version(a3);\
                break;\
            }\
            \
          case FUN_ENUM_insert_inode_hash_ac:\
            {\
                a1 = insert_inode_hash_ac(a3);\
                break;\
            }\
            \
          case FUN_ENUM_mark_inode_dirty_ac:\
            {\
                a1 = mark_inode_dirty_ac(a3);\
                break;\
            }\
            \
          case FUN_ENUM_sync_inode_cog:\
            {\
                a1 = sync_inode_cog(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = update_am_time(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t100;
#define FUN_DISP_MACRO_dispatch_t100(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_is_dir_ac:\
            {\
                a1 = is_dir_ac(a3);\
                break;\
            }\
            \
          case FUN_ENUM_is_dirsync_ac:\
            {\
                a1 = is_dirsync_ac(a3);\
                break;\
            }\
            \
          case FUN_ENUM_is_nfs:\
            {\
                a1 = is_nfs(a3);\
                break;\
            }\
            \
          case FUN_ENUM_is_null_0:\
            {\
                a1 = is_null_0(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = is_root_ino(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t101;
#define FUN_DISP_MACRO_dispatch_t101(a1, a2, a3)\
{\
    {\
        a1 = fat_dir_empty_ac(a3);\
    }\
}
typedef untyped_func_enum t102;
#define FUN_DISP_MACRO_dispatch_t102(a1, a2, a3)\
{\
    {\
        a1 = is_err(a3);\
    }\
}
typedef untyped_func_enum t103;
#define FUN_DISP_MACRO_dispatch_t103(a1, a2, a3)\
{\
    {\
        a1 = get_logstart(a3);\
    }\
}
typedef untyped_func_enum t104;
#define FUN_DISP_MACRO_dispatch_t104(a1, a2, a3)\
{\
    {\
        a1 = fat_dir_hash_cog(a3);\
    }\
}
typedef untyped_func_enum t105;
#define FUN_DISP_MACRO_dispatch_t105(a1, a2, a3)\
{\
    {\
        a1 = d_find_alias_ac(a3);\
    }\
}
typedef untyped_func_enum t106;
#define FUN_DISP_MACRO_dispatch_t106(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_get_dhead:\
            {\
                a1 = get_dhead(a3);\
                break;\
            }\
            \
          case FUN_ENUM_get_dir_hash:\
            {\
                a1 = get_dir_hash(a3);\
                break;\
            }\
            \
          case FUN_ENUM_get_fat_hash:\
            {\
                a1 = get_fat_hash(a3);\
                break;\
            }\
            \
          case FUN_ENUM_get_head:\
            {\
                a1 = get_head(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = get_version(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t107;
#define FUN_DISP_MACRO_dispatch_t107(a1, a2, a3)\
{\
    {\
        a1 = iput_ac(a3);\
    }\
}
typedef untyped_func_enum t108;
#define FUN_DISP_MACRO_dispatch_t108(a1, a2, a3)\
{\
    {\
        a1 = ptr_greq_ac(a3);\
    }\
}
typedef untyped_func_enum t109;
#define FUN_DISP_MACRO_dispatch_t109(a1, a2, a3)\
{\
    {\
        a1 = incr_head_pointer(a3);\
    }\
}
typedef untyped_func_enum t110;
#define FUN_DISP_MACRO_dispatch_t110(a1, a2, a3)\
{\
    {\
        a1 = fat_msg_ac(a3);\
    }\
}
typedef untyped_func_enum t111;
#define FUN_DISP_MACRO_dispatch_t111(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_spdir_unlock:\
            {\
                a1 = spdir_unlock(a3);\
                break;\
            }\
            \
          case FUN_ENUM_spinode_unlock:\
            {\
                a1 = spinode_unlock(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = ulock(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t112;
#define FUN_DISP_MACRO_dispatch_t112(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_flock_buildinode:\
            {\
                a1 = flock_buildinode(a3);\
                break;\
            }\
            \
          case FUN_ENUM_funlock_buildinode:\
            {\
                a1 = funlock_buildinode(a3);\
                break;\
            }\
            \
          case FUN_ENUM_slock:\
            {\
                a1 = slock(a3);\
                break;\
            }\
            \
          case FUN_ENUM_spdir_lock:\
            {\
                a1 = spdir_lock(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = spinode_lock(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t113;
#define FUN_DISP_MACRO_dispatch_t113(a1, a2, a3)\
{\
    {\
        a1 = set_ino(a3);\
    }\
}
typedef untyped_func_enum t114;
#define FUN_DISP_MACRO_dispatch_t114(a1, a2, a3)\
{\
    {\
        a1 = deep_fat_remove_entries_ac(a3);\
    }\
}
typedef untyped_func_enum t115;
#define FUN_DISP_MACRO_dispatch_t115(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_fat_add_entries_ac:\
            {\
                a1 = fat_add_entries_ac(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = fat_search_long_ac(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t116;
#define FUN_DISP_MACRO_dispatch_t116(a1, a2, a3)\
{\
    {\
        a1 = last_char_dot_ac(a3);\
    }\
}
typedef untyped_func_enum t117;
#define FUN_DISP_MACRO_dispatch_t117(a1, a2, a3)\
{\
    {\
        a1 = fat_iget_ac(a3);\
    }\
}
typedef untyped_func_enum t118;
#define FUN_DISP_MACRO_dispatch_t118(a1, a2, a3)\
{\
    {\
        a1 = vfat_build_slots_ac(a3);\
    }\
}
typedef untyped_func_enum t119;
#define FUN_DISP_MACRO_dispatch_t119(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_deep_fat_write_inode_ac:\
            {\
                a1 = deep_fat_write_inode_ac(a3);\
                break;\
            }\
            \
          case FUN_ENUM_fat_free_clusters_ac:\
            {\
                a1 = fat_free_clusters_ac(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = set_nlink_ac(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t120;
#define FUN_DISP_MACRO_dispatch_t120(a1, a2, a3)\
{\
    {\
        a1 = del_slots_cog(a3);\
    }\
}
typedef untyped_func_enum t38;
#define FUN_DISP_MACRO_dispatch_t38(a1, a2, a3)\
{\
    {\
        a1 = del_slots_gen(a3);\
    }\
}
typedef untyped_func_enum t42;
#define FUN_DISP_MACRO_dispatch_t42(a1, a2, a3)\
{\
    {\
        a1 = del_slots_cons(a3);\
    }\
}
typedef untyped_func_enum t121;
#define FUN_DISP_MACRO_dispatch_t121(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_hlist_add_head_ac:\
            {\
                a1 = hlist_add_head_ac(a3);\
                break;\
            }\
            \
          case FUN_ENUM_set_ipos:\
            {\
                a1 = set_ipos(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = set_version(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t122;
#define FUN_DISP_MACRO_dispatch_t122(a1, a2, a3)\
{\
    {\
        a1 = vfat_find_cog(a3);\
    }\
}
typedef untyped_func_enum t123;
#define FUN_DISP_MACRO_dispatch_t123(a1, a2, a3)\
{\
    {\
        a1 = fat_fill_inode_ac(a3);\
    }\
}
typedef untyped_func_enum t124;
#define FUN_DISP_MACRO_dispatch_t124(a1, a2, a3)\
{\
    {\
        a1 = mark_buffer_dirty_inode_ac(a3);\
    }\
}
typedef untyped_func_enum t73;
#define FUN_DISP_MACRO_dispatch_t73(a1, a2, a3)\
{\
    {\
        a1 = iterate_1(a3);\
    }\
}
typedef untyped_func_enum t49;
#define FUN_DISP_MACRO_dispatch_t49(a1, a2, a3)\
{\
    {\
        a1 = striptail_gen(a3);\
    }\
}
typedef untyped_func_enum t125;
#define FUN_DISP_MACRO_dispatch_t125(a1, a2, a3)\
{\
    {\
        a1 = d_splice_alias_ac(a3);\
    }\
}
typedef untyped_func_enum t126;
#define FUN_DISP_MACRO_dispatch_t126(a1, a2, a3)\
{\
    {\
        a1 = d_instantiate_ac(a3);\
    }\
}
typedef untyped_func_enum t127;
#define FUN_DISP_MACRO_dispatch_t127(a1, a2, a3)\
{\
    {\
        a1 = alias_cond(a3);\
    }\
}
typedef untyped_func_enum t53;
#define FUN_DISP_MACRO_dispatch_t53(a1, a2, a3)\
{\
    {\
        a1 = striptail_cons(a3);\
    }\
}
typedef untyped_func_enum t78;
#define FUN_DISP_MACRO_dispatch_t78(a1, a2, a3)\
{\
    {\
        a1 = iterate_0(a3);\
    }\
}
typedef untyped_func_enum t128;
#define FUN_DISP_MACRO_dispatch_t128(a1, a2, a3)\
{\
    {\
        a1 = setup_inode(a3);\
    }\
}
typedef untyped_func_enum t129;
#define FUN_DISP_MACRO_dispatch_t129(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_add_head_cog:\
            {\
                a1 = add_head_cog(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = fat_attach_cog(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t130;
#define FUN_DISP_MACRO_dispatch_t130(a1, a2, a3)\
{\
    {\
        a1 = fat_detach_cog(a3);\
    }\
}
typedef untyped_func_enum t131;
#define FUN_DISP_MACRO_dispatch_t131(a1, a2, a3)\
{\
    {\
        a1 = is_equal_den(a3);\
    }\
}
typedef untyped_func_enum t132;
#define FUN_DISP_MACRO_dispatch_t132(a1, a2, a3)\
{\
    {\
        a1 = d_move_ac(a3);\
    }\
}
typedef untyped_func_enum t133;
#define FUN_DISP_MACRO_dispatch_t133(a1, a2, a3)\
{\
    {\
        a1 = fat_build_inode_cog(a3);\
    }\
}
typedef untyped_func_enum t134;
#define FUN_DISP_MACRO_dispatch_t134(a1, a2, a3)\
{\
    {\
        a1 = set_d_time(a3);\
    }\
}
typedef untyped_func_enum t135;
#define FUN_DISP_MACRO_dispatch_t135(a1, a2, a3)\
{\
    {\
        a1 = fat_remove_entries_cog(a3);\
    }\
}
typedef untyped_func_enum t136;
#define FUN_DISP_MACRO_dispatch_t136(a1, a2, a3)\
{\
    {\
        a1 = vfat_add_entry_cog(a3);\
    }\
}
typedef untyped_func_enum t137;
#define FUN_DISP_MACRO_dispatch_t137(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_vfat_create_cog:\
            {\
                a1 = vfat_create_cog(a3);\
                break;\
            }\
            \
          case FUN_ENUM_vfat_mkdir_cog:\
            {\
                a1 = vfat_mkdir_cog(a3);\
                break;\
            }\
            \
          case FUN_ENUM_vfat_rmdir_cog:\
            {\
                a1 = vfat_rmdir_cog(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = vfat_unlink_cog(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t138;
#define FUN_DISP_MACRO_dispatch_t138(a1, a2, a3)\
{\
    {\
        a1 = vfat_lookup_cog(a3);\
    }\
}
typedef untyped_func_enum t139;
#define FUN_DISP_MACRO_dispatch_t139(a1, a2, a3)\
{\
    {\
        a1 = set_time(a3);\
    }\
}
typedef untyped_func_enum t140;
#define FUN_DISP_MACRO_dispatch_t140(a1, a2, a3)\
{\
    {\
        a1 = fat_alloc_new_dir_ac(a3);\
    }\
}
typedef untyped_func_enum t141;
#define FUN_DISP_MACRO_dispatch_t141(a1, a2, a3)\
{\
    {\
        a1 = decrementU32(a3);\
    }\
}
typedef untyped_func_enum t142;
#define FUN_DISP_MACRO_dispatch_t142(a1, a2, a3)\
{\
    {\
        a1 = hash_32_ac_0(a3);\
    }\
}
typedef untyped_func_enum t143;
#define FUN_DISP_MACRO_dispatch_t143(a1, a2, a3)\
{\
    {\
        a1 = get_qstr_name(a3);\
    }\
}
typedef untyped_func_enum t144;
#define FUN_DISP_MACRO_dispatch_t144(a1, a2, a3)\
{\
    {\
        a1 = vfat_striptail_len_cog(a3);\
    }\
}
typedef untyped_func_enum t145;
#define FUN_DISP_MACRO_dispatch_t145(a1, a2, a3)\
{\
    {\
        a1 = sync_dirty_buffer_ac(a3);\
    }\
}
typedef untyped_func_enum t146;
#define FUN_DISP_MACRO_dispatch_t146(a1, a2, a3)\
{\
    {\
        a1 = get_qstr_length(a3);\
    }\
}
typedef untyped_func_enum t147;
#define FUN_DISP_MACRO_dispatch_t147(a1, a2, a3)\
{\
    {\
        switch (a2) {\
            \
          case FUN_ENUM_brelse_ac:\
            {\
                a1 = brelse_ac(a3);\
                break;\
            }\
            \
          case FUN_ENUM_delete_first_ac:\
            {\
                a1 = delete_first_ac(a3);\
                break;\
            }\
            \
          case FUN_ENUM_fat_hash_cog:\
            {\
                a1 = fat_hash_cog(a3);\
                break;\
            }\
            \
          case FUN_ENUM_get_b_data:\
            {\
                a1 = get_b_data(a3);\
                break;\
            }\
            \
          default:\
            {\
                a1 = hash_32_ac_1(a3);\
                break;\
            }\
        }\
    }\
}
typedef untyped_func_enum t148;
#define FUN_DISP_MACRO_dispatch_t148(a1, a2, a3)\
{\
    {\
        a1 = hlist_del_init_ac(a3);\
    }\
}
typedef untyped_func_enum t149;
#define FUN_DISP_MACRO_dispatch_t149(a1, a2, a3)\
{\
    {\
        a1 = noent_ac(a3);\
    }\
}
typedef untyped_func_enum t150;
#define FUN_DISP_MACRO_dispatch_t150(a1, a2, a3)\
{\
    {\
        a1 = get_null_inode_ac(a3);\
    }\
}
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
#endif


