"""Protocol constants matching the C++ aether::ipc definitions."""

# ---- Protocol ---------------------------------------------------------------

PROTOCOL_VERSION = 1

RING_CAPACITY = 256 * 1024          # 262144 bytes per direction (power of 2)
RING_MASK = RING_CAPACITY - 1

# Cache-line size used by the ouroboros SPSC ControlBlock.
CACHE_LINE_SIZE = 64

# ControlBlock layout (aligned to CACHE_LINE_SIZE):
#   [0  ..  3]  head  (atomic uint32)
#   [4  .. 63]  pad1
#   [64 .. 67]  tail  (atomic uint32)
#   [68 ..127]  pad2
CONTROL_BLOCK_SIZE = 2 * CACHE_LINE_SIZE       # 128 bytes

# Byte offset of the data region inside one IpcRing.
DATA_OFFSET = CONTROL_BLOCK_SIZE               # 128

# Total size of one IpcRing (ControlBlock + data).
IPCRING_SIZE = CONTROL_BLOCK_SIZE + RING_CAPACITY  # 262272

# Shared memory holds two rings: Ring 0 (client->server) then Ring 1 (server->client).
SHM_SIZE = 2 * IPCRING_SIZE                   # 524544

# Offsets of head and tail inside the ControlBlock.
HEAD_OFFSET = 0
TAIL_OFFSET = CACHE_LINE_SIZE                  # 64

# ---- Frame Header -----------------------------------------------------------

FRAME_HEADER_SIZE = 24
FRAME_HEADER_FORMAT = "<HHIIIIi"   # little-endian, matches x86 native layout

# Maximum payload that fits in a single frame.
MAX_PAYLOAD = RING_CAPACITY - FRAME_HEADER_SIZE

# ---- Frame Flags ------------------------------------------------------------

FRAME_REQUEST  = 0x0001
FRAME_RESPONSE = 0x0002
FRAME_NOTIFY   = 0x0004

# ---- Error Codes ------------------------------------------------------------

IPC_SUCCESS             =  0
IPC_ERR_DISCONNECTED    = -1
IPC_ERR_TIMEOUT         = -2
IPC_ERR_INVALID_SERVICE = -3
IPC_ERR_INVALID_METHOD  = -4
IPC_ERR_VERSION_MISMATCH = -5
IPC_ERR_RING_FULL       = -6
IPC_ERR_STOPPED         = -7
IPC_ERR_INVALID_ARGUMENT = -8
IPC_ERR_TRANSPORT       = -9
IPC_ERR_CRC             = -10
IPC_ERR_NOT_SUPPORTED   = -11
IPC_ERR_NO_SPACE        = -12
IPC_ERR_OVERFLOW        = -13

# ---- Handshake service IDs --------------------------------------------------

HANDSHAKE_SERVICE_ID = 0x00000000
HANDSHAKE_HELLO      = 0x00000001
HANDSHAKE_ACK        = 0x00000002

# ---- SharedMemoryHandshake layout -------------------------------------------
# struct { uint16_t version; uint16_t reserved; char shmName[128]; }

SHM_HANDSHAKE_FORMAT = "<HH128s"
SHM_HANDSHAKE_SIZE = 132

# ---- Socket timeout ---------------------------------------------------------

SOCKET_TIMEOUT_MS = 5000
