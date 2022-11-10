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

#[derive(Debug, PartialEq)]
pub enum PageLocation {
    MEMORY(usize),
    SWAP(usize, usize),
    NONE,
}

macro_rules! bitmatch {
    ($pattern:literal) => {
        value if (value & $pattern) != 0
    };
}

impl PageLocation {
    pub fn new(value: u64) -> Self {
        return match value {
            bitmatch!(1 << 63) => PageLocation::MEMORY((value & ((1 << 55) - 1)) as usize),
            bitmatch!(1 << 62) => PageLocation::SWAP(
                (value & 0b0001_1111) as usize,
                ((value >> 5) & ((1 << 50) - 1)) as usize
            ),
            _ => PageLocation::NONE,
        };
    }
}

#[derive(Debug)]
pub struct PageFrame {
    location: PageLocation,
    is_file: bool,
    is_dirty: bool,
}

impl PageFrame {
    pub fn new(value: u64) -> Self {
        return Self {
            location: PageLocation::new(value),
            is_file: value & (1 << 61) != 0,
            is_dirty: value & (55 << 1) != 0,
        };
    }
}

#[derive(Debug)]
pub struct PageFrameRegion {
    frame: PageFrame,
    len: usize,
}

impl PageFrameRegion {
    pub fn new(value: u64, len: usize) -> Self {
        return Self {
            frame: PageFrame::new(value),
            len,
        };
    }
}

#[derive(Debug)]
pub struct ProcVirtualMapRegion {
    address_start: usize,
    address_end: usize,
    permissions: Permissions,
    offset: usize,
    physical_regions: HashMap<usize, PageFrameRegion>,
    path: Option<String>,
}
