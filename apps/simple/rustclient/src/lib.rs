// Copyright 2016, NICTA
//
// This software may be distributed and modified according to the terms of
// the BSD 2-Clause license. Note that NO WARRANTY is provided.
// See "LICENSE_BSD2.txt" for details.
//
// @TAG(NICTA_BSD)
//

// For no_std: 
// Note that you do need either libc or eh_personality and panic_fmt. 
// Having both libc and manual definitions will be an error, 
// but having neither is also an error.


// This is the camkes entry point for this app
//#![feature(lang_items, libc)]
//extern crate libc;

// also works as no_std and without libc
//#![no_std]
// but no_std requires panic


#[no_mangle]
extern "C" {
  fn printf(val: *const i8);
}

#[no_mangle]
pub extern "C" fn run() -> isize {
    // println!("Hello, RUST!!");
    unsafe{ printf(b"Hello from rust\0".as_ptr() as *const i8); }
    0
}


