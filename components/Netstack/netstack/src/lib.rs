extern crate smoltcp;


use std::str;
use std::collections::BTreeMap;
use smoltcp::wire::{EthernetAddress, IpAddress, IpCidr};
use smoltcp::iface::{EthernetInterfaceBuilder, NeighborCache};
use smoltcp::socket::SocketSet;
use smoltcp::socket::{UdpSocketBuffer, UdpSocket, UdpPacketMetadata};
use smoltcp::time::Instant;
use smoltcp::phy::Sel4Device;


#[no_mangle]
extern "C" {
  fn printf(val: *const i8);
}


#[no_mangle]
pub extern "C" fn run() -> isize {
    unsafe{ printf(b"Hello from Rust, starting main\n\0".as_ptr() as *const i8); }


    main();

    unsafe{ printf(b"Main done\n\0".as_ptr() as *const i8); }
    0
}

extern "C" {
    fn ethdriver_mac(b1: *mut u8, b2: *mut u8, b3: *mut u8, b4: *mut u8, b5: *mut u8, b6: *mut u8);
}


/// Event callback I believe
/// `badge` is not used
#[no_mangle]
pub extern "C" fn ethdriver_has_data_callback(_badge: u32) {}

/// Pass the device MAC address to the callee
fn get_device_mac() -> EthernetAddress {
    let mut b1: u8 = 0;
    let mut b2: u8 = 0;
    let mut b3: u8 = 0;
    let mut b4: u8 = 0;
    let mut b5: u8 = 0;
    let mut b6: u8 = 0;

    unsafe {
        ethdriver_mac(&mut b1, &mut b2, &mut b3, &mut b4, &mut b5, &mut b6);
    }

    EthernetAddress([b1, b2, b3, b4, b5, b6])
}


fn main() {
    unsafe{ printf(b"A\n\0".as_ptr() as *const i8); }
    let device = Sel4Device::new();
    unsafe{ printf(b"B\n\0".as_ptr() as *const i8); }
    let mut neighbor_cache_entries = [None; 8];
    let mut neighbor_cache = NeighborCache::new(&mut neighbor_cache_entries[..]);
    unsafe{ printf(b"C\n\0".as_ptr() as *const i8); }
    let udp1_rx_buffer = UdpSocketBuffer::new(vec![UdpPacketMetadata::EMPTY], vec![0; 64]);
    let udp1_tx_buffer = UdpSocketBuffer::new(vec![UdpPacketMetadata::EMPTY], vec![0; 128]);
    let udp1_socket = UdpSocket::new(udp1_rx_buffer, udp1_tx_buffer);
    unsafe{ printf(b"D\n\0".as_ptr() as *const i8); }
    let udp2_rx_buffer = UdpSocketBuffer::new(vec![UdpPacketMetadata::EMPTY], vec![0; 64]);
    let udp2_tx_buffer = UdpSocketBuffer::new(vec![UdpPacketMetadata::EMPTY], vec![0; 128]);
    let udp2_socket = UdpSocket::new(udp2_rx_buffer, udp2_tx_buffer);
    unsafe{ printf(b"E\n\0".as_ptr() as *const i8); }

    let ethernet_addr = get_device_mac();
    let ip_addrs = [IpCidr::new(IpAddress::v4(192, 168, 179, 2), 24)];
    let mut iface = EthernetInterfaceBuilder::new(device)
        .ethernet_addr(ethernet_addr)
        .neighbor_cache(neighbor_cache)
        .ip_addrs(ip_addrs)
        .finalize();

    let mut sockets = SocketSet::new(vec![]);
    let udp1_handle = sockets.add(udp1_socket);
    let udp2_handle = sockets.add(udp2_socket);

    loop {
        let timestamp = Instant::now();
        iface.poll(&mut sockets, timestamp).expect("poll error");

        // udp:6969: respond "hello"
        {
            let mut socket = sockets.get::<UdpSocket>(udp1_handle);
            if !socket.is_open() {
                socket.bind(6969).unwrap()
            }

            let client = match socket.recv() {
                Ok((data, endpoint)) => {
                    println!(
                        "udp:6969 recv data: {:?} from {}",
                        str::from_utf8(data.as_ref()).unwrap(),
                        endpoint
                    );
                    Some(endpoint)
                }
                Err(_) => None,
            };
            if let Some(endpoint) = client {
                let data = b"hello\n";
                println!(
                    "udp:6969 send data: {:?}",
                    str::from_utf8(data.as_ref()).unwrap()
                );
                socket.send_slice(data, endpoint).unwrap();
            }
        }

        // udp:6942: echo with reverse
        {
            let mut socket = sockets.get::<UdpSocket>(udp2_handle);
            if !socket.is_open() {
                socket.bind(6942).unwrap()
            }

            let mut rx_data = Vec::new();
            let client = match socket.recv() {
                Ok((data, endpoint)) => {
                    println!(
                        "udp:6969 recv data: {:?} from {}",
                        str::from_utf8(data.as_ref()).unwrap(),
                        endpoint
                    );
                    rx_data.extend_from_slice(data);
                    Some(endpoint)
                }
                Err(_) => None,
            };

            if let Some(endpoint) = client {
                if rx_data.len() > 0 {
                    let mut data = rx_data.split(|&b| b == b'\n').collect::<Vec<_>>().concat();
                    data.reverse();
                    data.extend(b"\n");
                    println!(
                        "udp:6942 send data: {:?}",
                        str::from_utf8(data.as_ref()).unwrap()
                    );
                    socket.send_slice(&data, endpoint).unwrap();
                }
            }
        }
    }
}

