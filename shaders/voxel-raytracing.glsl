bool checkBitmask(uvec2 bitmask, uvec3 location) {
    uint bitIndex = location.x | (location.y << 2u) | (location.z << 4u);
    
    if (bitIndex < 32u) {
        return 0u != (bitmask.x & (1u << bitIndex));
    } else {
        return 0u != (bitmask.y & (1u << (bitIndex - 32u)));
    }
}