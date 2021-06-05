use std::io::stdin;
use std::thread;
use std::time::Duration;
use std::{
    io::{Error, Read, Write},
    net::{Ipv4Addr, SocketAddrV4, TcpListener},
};

fn main() -> Result<(), Error> {
    // first thread - start up a TcpListener
    thread::spawn(|| {
        let socket = SocketAddrV4::new(Ipv4Addr::new(0, 0, 0, 0), 9123);
        let (mut stream, socket_addr) = match TcpListener::bind(socket) {
            Ok(l) => l.accept().unwrap(),
            _ => panic!("Failed to bind"),
        };

        loop {
            let mut buf: [u8; 100] = [0; 100];
            match stream.read(&mut buf) {
                Ok(bytes_read) => {
                    let output = std::str::from_utf8(&buf).unwrap().to_string();
                    println!(
                        "{:?} Read {} bytes from {:?}, data: {}",
                        thread::current().name(),
                        bytes_read,
                        socket_addr.ip(),
                        output
                    );
                }
                Err(_) => panic!(),
            }
        }
    });

    thread::sleep(Duration::from_millis(500));

    // second thread - client (TcpStream)
    thread::spawn(|| {
        let socket = SocketAddrV4::new(Ipv4Addr::new(127, 0, 0, 1), 9123);
        let mut s = match std::net::TcpStream::connect(socket) {
            Ok(l) => l,
            Err(_) => todo!(),
        };

        loop {
            thread::sleep(Duration::from_millis(500));
            
            let time = time::OffsetDateTime::now_utc().format("%d/%m/%Y %T");
            println!("Publishing: {}", time);
            s.write(time.as_bytes()).unwrap();
        }
    });

    let _ = stdin().read(&mut [0u8]).unwrap();
    Ok(())
}
