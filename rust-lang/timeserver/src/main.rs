use std::{
    io::{Error, Read},
    net::{Ipv4Addr, SocketAddrV4, TcpListener},
};

fn main() -> Result<(), Error> {
    println!("Hello, world!");

    let socket = SocketAddrV4::new(Ipv4Addr::new(0, 0, 0, 0), 9123);
    let (mut stream, socket_addr) = match TcpListener::bind(socket) {
        Ok(l) => l.accept().unwrap(),
        _ => panic!("Failed to bind"),
    };

    let mut buf: [u8; 100] = [0; 100];
    match stream.read(&mut buf) {
        Ok(bytes_read) => {
            println!("Read {} bytes from {:?}", bytes_read, socket_addr.ip());
        }
        Err(_) => panic!(),
    }

    Ok(())
}
