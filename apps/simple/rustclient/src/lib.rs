extern crate smoltcp;

use smoltcp::wire::{EthernetAddress};

#[no_mangle]
extern "C" {
  fn printf(val: *const i8);
}

#[no_mangle]
pub extern "C" fn run() -> isize {
    let x = Box::new(1);
    unsafe{ printf(format!("Hello from rust! Box={}\n\0",x).as_ptr() as *const i8); }

    let mut v = vec![];
    v.push(1);
    v.push(2);
    v.push(3);
    unsafe{ printf(format!("Hello from rust! Vec={:?}\n\0",v).as_ptr() as *const i8); }

    let ethernet_addr = EthernetAddress([0x02, 0x00, 0x00, 0x00, 0x00, 0x01]);
    unsafe{ printf(format!("Ethernet addr:s {}\n\0",ethernet_addr).as_ptr() as *const i8); }
    0
}

