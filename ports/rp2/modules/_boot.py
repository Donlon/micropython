import os
import machine, rp2


# Try to mount the filesystem, and format the flash if it doesn't exist.
# Note: the flash requires the programming size to be aligned to 256 bytes.
flash_bdev = rp2.Flash()
try:
    flash_vfs = os.VfsLfs2(flash_bdev, progsize=256)
except:
    os.VfsLfs2.mkfs(flash_bdev, progsize=256)
    flash_vfs = os.VfsLfs2(flash_bdev, progsize=256)
os.mount(flash_vfs, "/")

# initialize tmpfs
tmpfs_size = 0
try:
    with open('.tmpfs', 'r') as f:
        tmpfs_size = int(f.read())
except:
    pass

if tmpfs_size > 0:
    tmpfs_bdev = rp2.MemBlocks(size=tmpfs_size)
    os.VfsLfs2.mkfs(tmpfs_bdev, progsize=256)
    tmpfs_vfs = os.VfsLfs2(tmpfs_bdev, progsize=256)
    os.mount(tmpfs_vfs, "/tmp")

del os, flash_bdev, flash_vfs, tmpfs_size, tmpfs_bdev, tmpfs_vfs
