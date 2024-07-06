/*
 * Space spec/docs

 * X_ws: World Space, global space. modelMatrix * X_ms 
 * X_ms: Model Space, the local space of the vertex positions sent by the CPU.
 * X_ts: Tree Space, local space of a THC Tree node. Origin is voxel (0,0,0) on the tree, (x,y,z) model space is voxel (4x,4y,4z) on the tree
 * X_vs: Voxel Space, same as tree space but 1 unit = 1 voxel
*/

uvec2 readChunkBitmask(vec3 loc_ws) {
    return texelFetch(uChunkData, ivec3(floor(loc_ws)), 0).rg;
}
uvec2 readSubChunkBitmask(vec3 loc_ws) {
    return texelFetch(uSubChunkData, ivec3(floor(loc_ws*4.0)), 0).rg;
}

bool insideTHCTree(vec3 loc_ts) {
    return (loc_ts.x < 1.0) && (loc_ts.x > 0.0) && (loc_ts.y < 1.0) && (loc_ts.y > 0.0) && (loc_ts.z < 1.0) && (loc_ts.z > 0.0);
}
bool insideTHCTree(ivec3 loc_vs) {
    return (loc_vs.x < 4) && (loc_vs.x >= 0) && (loc_vs.y < 4) && (loc_vs.y >= 0) && (loc_vs.z < 4) && (loc_vs.z >= 0);
}

bool checkBitmask(uvec2 bitmask, uvec3 location_vs) {
    if (!insideTHCTree(ivec3(location_vs))) return false;
    
    uint bitIndex = location_vs.x | (location_vs.y << 2u) | (location_vs.z << 4u);
    
    return 0u != ((bitIndex < 32u ? bitmask.x : bitmask.y) & (1u << (bitIndex % 32u)));
}

bool checkBitmask(uvec2 bitmask, vec3 location_ts) {
    return checkBitmask(bitmask, uvec3(location_ts * 3.9999));
}
/*
bool singleChunkMarch(uvec2 thcMask, inout vec3 ro_ts, vec3 rd) {
    vec3 ro_vsf = ro_ts * 4.0;
    vec3 ro_vsf_fract = fract(ro_vsf);
    ivec3 ro_vs = ivec3(floor(ro_vsf));


	vec3 deltaDist = abs(vec3(1.0) / rd);
	ivec3 rayStep = ivec3(sign(rd));
	vec3 sideDist = (sign(rd) * (vec3(ro_vs) - ro_vsf) + (sign(rd) * 0.5) + 0.5) * deltaDist;

    bvec3 mask;
    for (int i = 0; i < 12; i++) {
        if (checkBitmask(thcMask, uvec3(ro_vs))) {
            ro_ts = vec3(ro_vs + ro_vsf_fract) / 4.0;
            return true;
        }
        
        mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
        sideDist += vec3(mask) * deltaDist;
        ro_vs += ivec3(mask) * rayStep;
        ro_vsf += vec3(mask) * vec3(rayStep);

        if (!insideTHCTree(ro_vs)) break;
    }

    ro_ts = vec3(ro_vs + ro_vsf_fract) / 4.0;

    return false;
}*/

bool singleChunkMarch(uvec2 thcMask, inout vec3 ro_ts, vec3 rd) {
    vec3 sd = sign(rd);

    vec3 ro_vs = ro_ts * 3.99 + 0.01;

    for (int i = 0; i <= 12; i++) {
        if (checkBitmask(thcMask, uvec3(floor(ro_vs)))) {
            ro_ts = ro_vs / 4.0;
            return true;
        }

        vec3 t = abs(fract(-sd * ro_vs)/rd);
        float mint = min(t.x, min(t.y, t.z));

        ro_vs += rd * (mint+0.005);

        if (!insideTHCTree(ivec3(floor(ro_vs)))) break;
    }
    ro_ts = ro_vs / 4.0;
    return false;
}

bool worldMarch(inout vec3 ro_ms, vec3 rd) {
    vec3 ro_vs = ro_ms*0.99+0.001;

    vec3 sd = sign(rd);

    vec3 t;
    float mint;
    for (int i = 0; i <= int(uWorldSize_chunks.x + uWorldSize_chunks.y + uWorldSize_chunks.z); i++) {
        uvec2 chunk = readChunkBitmask(vec3(ro_vs));

        if (chunk != uvec2(0)) {
            vec3 ro_ts = fract(ro_vs);

            vec3 ro_sc_vs = ro_ts * 3.99 + 0.001;

            for (int j = 0; j <= 12; j++) {
                if (checkBitmask(chunk, uvec3(ro_sc_vs))) {
                    uvec2 subChunk = readSubChunkBitmask(floor(ro_vs) + ro_sc_vs/4.0);
                    
                    vec3 ro_ssc_vs = fract(ro_sc_vs);
                    bool hit_sc = singleChunkMarch(subChunk, ro_ssc_vs, rd);
                        // this masterpiece of space conversion
                    ro_ms = floor(ro_vs) + (ro_sc_vs + (ro_ssc_vs - fract(ro_sc_vs))) / 4.0;

                    if (hit_sc) return true;
                }

                t = abs(fract(-sd * ro_sc_vs)/rd);
                mint = min(t.x, min(t.y, t.z));

                ro_sc_vs += rd * (mint+0.005);

                if (!insideTHCTree(ivec3(floor(ro_sc_vs)))) break;
            }
        }

        t = abs(fract(-sd * ro_vs)/rd);
        mint = min(t.x, min(t.y, t.z));
        ro_vs += rd * (mint+0.005);
    }
    return false;
}