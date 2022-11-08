use std::collections::HashMap;
use page_size;

lazy_static!{
    pub static PAGE_SIZE: usize = page_size::get();
}

bitflags!{
    pub struct Permissions: u8 {
        const READ = 0x01;
        const WRITE = 0x02;
        const EXECUTE = 0x04;
        const SHARED = 0x08;
    }
}

#[derive(Debug,PartialEq)]
enum Location {
    RAM(usize),
    SWAP(usize, usize),
    NONE,
}

#[derive(Debug)]
pub struct PageFrame {
    location: Location,
    is_file: bool,
    dirty: bool,
}

#[derive(Debug)]
pub struct PageFrameRegion {
    frame: PageFrame,
    size: usize,
}

#[derive(Debug)]
pub struct PageFrameMap(HashMap<usize, PageFrameRegion>);

#[derive(Debug)]
pub struct MapVirtMemRegion {
    start: usize,
    end: usize,
    permissions: Permissions,
    offset: usize,
    path: Option<String>,
    physical_regions: Option<PageFrameMap>,
}