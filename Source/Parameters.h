#pragma once
#include <algorithm>
#include <array>
#include <string_view>
namespace prism
{
enum Param : size_t
{
    timbre,
    attack,
    decay,
    sustain,
    release,
    cutoff,
    resonance,
    gain,
    osc1_level,
    osc1_pitch,
    osc1_fine,
    osc1_pan,
    osc1_unison,
    osc1_detune,
    osc1_spread,
    osc1_phase,
    osc1_random,
    osc1_warp,
    osc1_warpAmount,
    osc1_interaction,
    osc1_interactionAmount,
    osc1_route,
    osc2_table,
    osc2_level,
    osc2_pitch,
    osc2_fine,
    osc2_pan,
    osc2_unison,
    osc2_detune,
    osc2_spread,
    osc2_phase,
    osc2_random,
    osc2_warp,
    osc2_warpAmount,
    osc2_interaction,
    osc2_interactionAmount,
    osc2_route,
    osc3_table,
    osc3_level,
    osc3_pitch,
    osc3_fine,
    osc3_pan,
    osc3_unison,
    osc3_detune,
    osc3_spread,
    osc3_phase,
    osc3_random,
    osc3_warp,
    osc3_warpAmount,
    osc3_interaction,
    osc3_interactionAmount,
    osc3_route,
    sub_level,
    sub_wave,
    sub_octave,
    sub_pan,
    noise_level,
    noise_colour,
    noise_pan,
    filter1_type,
    filter1_drive,
    filter1_key,
    filter2_cutoff,
    filter2_res,
    filter2_type,
    filter2_drive,
    filter2_key,
    filter2_enabled,
    filter_path,
    filter_mix,
    polyphony,
    mono,
    legato,
    retrigger,
    glide,
    seed,
    env2_attack,
    env2_decay,
    env2_sustain,
    env2_release,
    env3_attack,
    env3_decay,
    env3_sustain,
    env3_release,
    env4_attack,
    env4_decay,
    env4_sustain,
    env4_release,
    lfo1_rate,
    lfo1_shape,
    lfo1_sync,
    lfo1_division,
    lfo1_retrigger,
    lfo1_phase,
    lfo1_oneShot,
    lfo1_unipolar,
    lfo2_rate,
    lfo2_shape,
    lfo2_sync,
    lfo2_division,
    lfo2_retrigger,
    lfo2_phase,
    lfo2_oneShot,
    lfo2_unipolar,
    lfo3_rate,
    lfo3_shape,
    lfo3_sync,
    lfo3_division,
    lfo3_retrigger,
    lfo3_phase,
    lfo3_oneShot,
    lfo3_unipolar,
    lfo4_rate,
    lfo4_shape,
    lfo4_sync,
    lfo4_division,
    lfo4_retrigger,
    lfo4_phase,
    lfo4_oneShot,
    lfo4_unipolar,
    lfo5_rate,
    lfo5_shape,
    lfo5_sync,
    lfo5_division,
    lfo5_retrigger,
    lfo5_phase,
    lfo5_oneShot,
    lfo5_unipolar,
    lfo6_rate,
    lfo6_shape,
    lfo6_sync,
    lfo6_division,
    lfo6_retrigger,
    lfo6_phase,
    lfo6_oneShot,
    lfo6_unipolar,
    lfo7_rate,
    lfo7_shape,
    lfo7_sync,
    lfo7_division,
    lfo7_retrigger,
    lfo7_phase,
    lfo7_oneShot,
    lfo7_unipolar,
    lfo8_rate,
    lfo8_shape,
    lfo8_sync,
    lfo8_division,
    lfo8_retrigger,
    lfo8_phase,
    lfo8_oneShot,
    lfo8_unipolar,
    lfo9_rate,
    lfo9_shape,
    lfo9_sync,
    lfo9_division,
    lfo9_retrigger,
    lfo9_phase,
    lfo9_oneShot,
    lfo9_unipolar,
    lfo10_rate,
    lfo10_shape,
    lfo10_sync,
    lfo10_division,
    lfo10_retrigger,
    lfo10_phase,
    lfo10_oneShot,
    lfo10_unipolar,
    macro1,
    macro2,
    macro3,
    macro4,
    macro5,
    macro6,
    macro7,
    macro8,
    mod1_source,
    mod1_destination,
    mod1_amount,
    mod1_polarity,
    mod1_bypass,
    mod1_curve,
    mod2_source,
    mod2_destination,
    mod2_amount,
    mod2_polarity,
    mod2_bypass,
    mod2_curve,
    mod3_source,
    mod3_destination,
    mod3_amount,
    mod3_polarity,
    mod3_bypass,
    mod3_curve,
    mod4_source,
    mod4_destination,
    mod4_amount,
    mod4_polarity,
    mod4_bypass,
    mod4_curve,
    mod5_source,
    mod5_destination,
    mod5_amount,
    mod5_polarity,
    mod5_bypass,
    mod5_curve,
    mod6_source,
    mod6_destination,
    mod6_amount,
    mod6_polarity,
    mod6_bypass,
    mod6_curve,
    mod7_source,
    mod7_destination,
    mod7_amount,
    mod7_polarity,
    mod7_bypass,
    mod7_curve,
    mod8_source,
    mod8_destination,
    mod8_amount,
    mod8_polarity,
    mod8_bypass,
    mod8_curve,
    mod9_source,
    mod9_destination,
    mod9_amount,
    mod9_polarity,
    mod9_bypass,
    mod9_curve,
    mod10_source,
    mod10_destination,
    mod10_amount,
    mod10_polarity,
    mod10_bypass,
    mod10_curve,
    mod11_source,
    mod11_destination,
    mod11_amount,
    mod11_polarity,
    mod11_bypass,
    mod11_curve,
    mod12_source,
    mod12_destination,
    mod12_amount,
    mod12_polarity,
    mod12_bypass,
    mod12_curve,
    mod13_source,
    mod13_destination,
    mod13_amount,
    mod13_polarity,
    mod13_bypass,
    mod13_curve,
    mod14_source,
    mod14_destination,
    mod14_amount,
    mod14_polarity,
    mod14_bypass,
    mod14_curve,
    mod15_source,
    mod15_destination,
    mod15_amount,
    mod15_polarity,
    mod15_bypass,
    mod15_curve,
    mod16_source,
    mod16_destination,
    mod16_amount,
    mod16_polarity,
    mod16_bypass,
    mod16_curve,
    sat_enabled,
    sat_drive,
    sat_mix,
    chorus_enabled,
    chorus_rate,
    chorus_depth,
    chorus_mix,
    phaser_enabled,
    phaser_rate,
    phaser_depth,
    phaser_mix,
    delay_enabled,
    delay_time,
    delay_feedback,
    delay_mix,
    delay_sync,
    delay_division,
    reverb_enabled,
    reverb_size,
    reverb_damping,
    reverb_mix,
    eq_enabled,
    eq_low,
    eq_mid,
    eq_high,
    eq_frequency,
    comp_enabled,
    comp_threshold,
    comp_ratio,
    comp_attack,
    comp_release,
    comp_mix,
    limiter_enabled,
    limiter_ceiling,
    fx_slot0,
    fx_slot1,
    fx_slot2,
    fx_slot3,
    fx_slot4,
    fx_slot5,
    fx_slot6,
    fx_slot7,
    hybrid1_mode,
    hybrid1_root,
    hybrid1_start,
    hybrid1_end,
    hybrid1_reverse,
    hybrid1_loop,
    hybrid1_crossfade,
    hybrid1_sampleGain,
    hybrid1_grainPosition,
    hybrid1_grainSize,
    hybrid1_grainDensity,
    hybrid1_grainPitch,
    hybrid1_grainSpread,
    hybrid1_grainJitter,
    hybrid1_grainMotion,
    hybrid1_spectralPosition,
    hybrid1_spectralTilt,
    hybrid1_spectralHarmonics,
    hybrid2_mode,
    hybrid2_root,
    hybrid2_start,
    hybrid2_end,
    hybrid2_reverse,
    hybrid2_loop,
    hybrid2_crossfade,
    hybrid2_sampleGain,
    hybrid2_grainPosition,
    hybrid2_grainSize,
    hybrid2_grainDensity,
    hybrid2_grainPitch,
    hybrid2_grainSpread,
    hybrid2_grainJitter,
    hybrid2_grainMotion,
    hybrid2_spectralPosition,
    hybrid2_spectralTilt,
    hybrid2_spectralHarmonics,
    hybrid3_mode,
    hybrid3_root,
    hybrid3_start,
    hybrid3_end,
    hybrid3_reverse,
    hybrid3_loop,
    hybrid3_crossfade,
    hybrid3_sampleGain,
    hybrid3_grainPosition,
    hybrid3_grainSize,
    hybrid3_grainDensity,
    hybrid3_grainPitch,
    hybrid3_grainSpread,
    hybrid3_grainJitter,
    hybrid3_grainMotion,
    hybrid3_spectralPosition,
    hybrid3_spectralTilt,
    hybrid3_spectralHarmonics,
    sub_route,
    noise_route,
    mpe,
    mpeRange,
    internalTempo,
    arp_enabled,
    arp_order,
    arp_octaves,
    arp_division,
    arp_gate,
    arp_swing,
    arp_latch,
    seq_enabled,
    seq_length,
    seq_division,
    seq_swing,
    seq_transpose,
    seq_latch,
    step1_note,
    step1_velocity,
    step1_gate,
    step1_probability,
    step2_note,
    step2_velocity,
    step2_gate,
    step2_probability,
    step3_note,
    step3_velocity,
    step3_gate,
    step3_probability,
    step4_note,
    step4_velocity,
    step4_gate,
    step4_probability,
    step5_note,
    step5_velocity,
    step5_gate,
    step5_probability,
    step6_note,
    step6_velocity,
    step6_gate,
    step6_probability,
    step7_note,
    step7_velocity,
    step7_gate,
    step7_probability,
    step8_note,
    step8_velocity,
    step8_gate,
    step8_probability,
    step9_note,
    step9_velocity,
    step9_gate,
    step9_probability,
    step10_note,
    step10_velocity,
    step10_gate,
    step10_probability,
    step11_note,
    step11_velocity,
    step11_gate,
    step11_probability,
    step12_note,
    step12_velocity,
    step12_gate,
    step12_probability,
    step13_note,
    step13_velocity,
    step13_gate,
    step13_probability,
    step14_note,
    step14_velocity,
    step14_gate,
    step14_probability,
    step15_note,
    step15_velocity,
    step15_gate,
    step15_probability,
    step16_note,
    step16_velocity,
    step16_gate,
    step16_probability,
    env1_attackCurve,
    env1_decayCurve,
    env1_releaseCurve,
    env2_attackCurve,
    env2_decayCurve,
    env2_releaseCurve,
    env3_attackCurve,
    env3_decayCurve,
    env3_releaseCurve,
    env4_attackCurve,
    env4_decayCurve,
    env4_releaseCurve,
    lfo1_point1,
    lfo1_point2,
    lfo1_point3,
    lfo1_point4,
    lfo1_point5,
    lfo1_point6,
    lfo1_point7,
    lfo1_point8,
    lfo2_point1,
    lfo2_point2,
    lfo2_point3,
    lfo2_point4,
    lfo2_point5,
    lfo2_point6,
    lfo2_point7,
    lfo2_point8,
    lfo3_point1,
    lfo3_point2,
    lfo3_point3,
    lfo3_point4,
    lfo3_point5,
    lfo3_point6,
    lfo3_point7,
    lfo3_point8,
    lfo4_point1,
    lfo4_point2,
    lfo4_point3,
    lfo4_point4,
    lfo4_point5,
    lfo4_point6,
    lfo4_point7,
    lfo4_point8,
    lfo5_point1,
    lfo5_point2,
    lfo5_point3,
    lfo5_point4,
    lfo5_point5,
    lfo5_point6,
    lfo5_point7,
    lfo5_point8,
    lfo6_point1,
    lfo6_point2,
    lfo6_point3,
    lfo6_point4,
    lfo6_point5,
    lfo6_point6,
    lfo6_point7,
    lfo6_point8,
    lfo7_point1,
    lfo7_point2,
    lfo7_point3,
    lfo7_point4,
    lfo7_point5,
    lfo7_point6,
    lfo7_point7,
    lfo7_point8,
    lfo8_point1,
    lfo8_point2,
    lfo8_point3,
    lfo8_point4,
    lfo8_point5,
    lfo8_point6,
    lfo8_point7,
    lfo8_point8,
    lfo9_point1,
    lfo9_point2,
    lfo9_point3,
    lfo9_point4,
    lfo9_point5,
    lfo9_point6,
    lfo9_point7,
    lfo9_point8,
    lfo10_point1,
    lfo10_point2,
    lfo10_point3,
    lfo10_point4,
    lfo10_point5,
    lfo10_point6,
    lfo10_point7,
    lfo10_point8,
    osc1_warp2,
    osc1_warp2Amount,
    osc2_warp2,
    osc2_warp2Amount,
    osc3_warp2,
    osc3_warp2Amount,
    hybrid1_oneShot,
    hybrid2_oneShot,
    hybrid3_oneShot,
    paramCount
};
struct Parameter
{
    const char *id;
    const char *name;
    const char *unit;
    float min, max, def, skew, step;
};
inline constexpr std::array<Parameter, paramCount> parameters{
    {{"timbre", "Table position", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"attack", "Attack", "s", 0.001f, 5.0f, 0.01f, 0.3f, 0.0f},
     {"decay", "Decay", "s", 0.001f, 5.0f, 0.2f, 0.3f, 0.0f},
     {"sustain", "Sustain", "", 0.0f, 1.0f, 0.7f, 1.0f, 0.0f},
     {"release", "Release", "s", 0.001f, 10.0f, 0.3f, 0.3f, 0.0f},
     {"cutoff", "Cutoff", "Hz", 20.0f, 20000.0f, 8000.0f, 0.25f, 0.0f},
     {"resonance", "Resonance", "", 0.0f, 1.0f, 0.15f, 1.0f, 0.0f},
     {"gain", "Master", "", 0.0f, 1.0f, 0.5f, 1.0f, 0.0f},
     {"osc1_level", "Osc 1 Level", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"osc1_pitch", "Osc 1 Semitones", "st", -36.0f, 36.0f, 0.0f, 1.0f, 1.0f},
     {"osc1_fine", "Osc 1 Fine", "ct", -100.0f, 100.0f, 0.0f, 1.0f, 0.0f},
     {"osc1_pan", "Osc 1 Pan", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc1_unison", "Osc 1 Unison", "", 1.0f, 7.0f, 1.0f, 1.0f, 1.0f},
     {"osc1_detune", "Osc 1 Detune", "ct", 0.0f, 70.0f, 12.0f, 1.0f, 0.0f},
     {"osc1_spread", "Osc 1 Spread", "", 0.0f, 1.0f, 0.8f, 1.0f, 0.0f},
     {"osc1_phase", "Osc 1 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc1_random", "Osc 1 Phase random", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc1_warp", "Osc 1 Warp type", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"osc1_warpAmount", "Osc 1 Warp amount", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc1_interaction", "Osc 1 Interaction", "", 0.0f, 2.0f, 0.0f, 1.0f, 1.0f},
     {"osc1_interactionAmount", "Osc 1 Interaction depth", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc1_route", "Osc 1 Filter routing", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"osc2_table", "Osc 2 table", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"osc2_level", "Osc 2 Level", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc2_pitch", "Osc 2 Semitones", "st", -36.0f, 36.0f, 0.0f, 1.0f, 1.0f},
     {"osc2_fine", "Osc 2 Fine", "ct", -100.0f, 100.0f, 0.0f, 1.0f, 0.0f},
     {"osc2_pan", "Osc 2 Pan", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc2_unison", "Osc 2 Unison", "", 1.0f, 7.0f, 1.0f, 1.0f, 1.0f},
     {"osc2_detune", "Osc 2 Detune", "ct", 0.0f, 70.0f, 12.0f, 1.0f, 0.0f},
     {"osc2_spread", "Osc 2 Spread", "", 0.0f, 1.0f, 0.8f, 1.0f, 0.0f},
     {"osc2_phase", "Osc 2 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc2_random", "Osc 2 Phase random", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc2_warp", "Osc 2 Warp type", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"osc2_warpAmount", "Osc 2 Warp amount", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc2_interaction", "Osc 2 Interaction", "", 0.0f, 2.0f, 0.0f, 1.0f, 1.0f},
     {"osc2_interactionAmount", "Osc 2 Interaction depth", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc2_route", "Osc 2 Filter routing", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"osc3_table", "Osc 3 table", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"osc3_level", "Osc 3 Level", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc3_pitch", "Osc 3 Semitones", "st", -36.0f, 36.0f, 0.0f, 1.0f, 1.0f},
     {"osc3_fine", "Osc 3 Fine", "ct", -100.0f, 100.0f, 0.0f, 1.0f, 0.0f},
     {"osc3_pan", "Osc 3 Pan", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc3_unison", "Osc 3 Unison", "", 1.0f, 7.0f, 1.0f, 1.0f, 1.0f},
     {"osc3_detune", "Osc 3 Detune", "ct", 0.0f, 70.0f, 12.0f, 1.0f, 0.0f},
     {"osc3_spread", "Osc 3 Spread", "", 0.0f, 1.0f, 0.8f, 1.0f, 0.0f},
     {"osc3_phase", "Osc 3 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc3_random", "Osc 3 Phase random", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc3_warp", "Osc 3 Warp type", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"osc3_warpAmount", "Osc 3 Warp amount", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc3_interaction", "Osc 3 Interaction", "", 0.0f, 2.0f, 0.0f, 1.0f, 1.0f},
     {"osc3_interactionAmount", "Osc 3 Interaction depth", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc3_route", "Osc 3 Filter routing", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"sub_level", "Sub level", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"sub_wave", "Sub waveform", "", 0.0f, 2.0f, 0.0f, 1.0f, 1.0f},
     {"sub_octave", "Sub octave", "oct", -3.0f, 0.0f, -1.0f, 1.0f, 1.0f},
     {"sub_pan", "Sub pan", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"noise_level", "Noise level", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"noise_colour", "Noise colour", "", 0.0f, 1.0f, 0.5f, 1.0f, 0.0f},
     {"noise_pan", "Noise pan", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"filter1_type", "Filter 1 type", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"filter1_drive", "Filter 1 drive", "dB", 0.0f, 24.0f, 0.0f, 1.0f, 0.0f},
     {"filter1_key", "Filter 1 tracking", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"filter2_cutoff", "Filter 2 cutoff", "Hz", 20.0f, 20000.0f, 8000.0f, 0.25f, 0.0f},
     {"filter2_res", "Filter 2 resonance", "", 0.0f, 1.0f, 0.15f, 1.0f, 0.0f},
     {"filter2_type", "Filter 2 type", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"filter2_drive", "Filter 2 drive", "dB", 0.0f, 24.0f, 0.0f, 1.0f, 0.0f},
     {"filter2_key", "Filter 2 tracking", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"filter2_enabled", "Filter 2 enabled", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"filter_path", "Filter path", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"filter_mix", "Filter blend", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"polyphony", "Polyphony", "", 1.0f, 16.0f, 16.0f, 1.0f, 1.0f},
     {"mono", "Mono", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"legato", "Legato", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {"retrigger", "Retrigger", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"glide", "Portamento", "s", 0.0f, 2.0f, 0.0f, 0.4f, 0.0f},
     {"seed", "Random seed", "", 1.0f, 65535.0f, 1979.0f, 1.0f, 1.0f},
     {"env2_attack", "Env 2 Attack", "s", 0.001f, 5.0f, 0.02f, 0.3f, 0.0f},
     {"env2_decay", "Env 2 Decay", "s", 0.001f, 5.0f, 0.4f, 0.3f, 0.0f},
     {"env2_sustain", "Env 2 Sustain", "", 0.0f, 1.0f, 0.5f, 1.0f, 0.0f},
     {"env2_release", "Env 2 Release", "s", 0.001f, 10.0f, 0.5f, 0.3f, 0.0f},
     {"env3_attack", "Env 3 Attack", "s", 0.001f, 5.0f, 0.02f, 0.3f, 0.0f},
     {"env3_decay", "Env 3 Decay", "s", 0.001f, 5.0f, 0.4f, 0.3f, 0.0f},
     {"env3_sustain", "Env 3 Sustain", "", 0.0f, 1.0f, 0.5f, 1.0f, 0.0f},
     {"env3_release", "Env 3 Release", "s", 0.001f, 10.0f, 0.5f, 0.3f, 0.0f},
     {"env4_attack", "Env 4 Attack", "s", 0.001f, 5.0f, 0.02f, 0.3f, 0.0f},
     {"env4_decay", "Env 4 Decay", "s", 0.001f, 5.0f, 0.4f, 0.3f, 0.0f},
     {"env4_sustain", "Env 4 Sustain", "", 0.0f, 1.0f, 0.5f, 1.0f, 0.0f},
     {"env4_release", "Env 4 Release", "s", 0.001f, 10.0f, 0.5f, 0.3f, 0.0f},
     {"lfo1_rate", "LFO 1 Rate", "Hz", 0.02f, 40.0f, 1.0f, 0.3f, 0.0f},
     {"lfo1_shape", "LFO 1 Shape", "", 0.0f, 5.0f, 0.0f, 1.0f, 1.0f},
     {"lfo1_sync", "LFO 1 Sync", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo1_division", "LFO 1 Division", "", 0.0f, 8.0f, 4.0f, 1.0f, 1.0f},
     {"lfo1_retrigger", "LFO 1 Retrigger", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {"lfo1_phase", "LFO 1 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo1_oneShot", "LFO 1 One shot", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo1_unipolar", "LFO 1 Unipolar", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo2_rate", "LFO 2 Rate", "Hz", 0.02f, 40.0f, 1.0f, 0.3f, 0.0f},
     {"lfo2_shape", "LFO 2 Shape", "", 0.0f, 5.0f, 0.0f, 1.0f, 1.0f},
     {"lfo2_sync", "LFO 2 Sync", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo2_division", "LFO 2 Division", "", 0.0f, 8.0f, 4.0f, 1.0f, 1.0f},
     {"lfo2_retrigger", "LFO 2 Retrigger", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {"lfo2_phase", "LFO 2 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo2_oneShot", "LFO 2 One shot", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo2_unipolar", "LFO 2 Unipolar", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo3_rate", "LFO 3 Rate", "Hz", 0.02f, 40.0f, 1.0f, 0.3f, 0.0f},
     {"lfo3_shape", "LFO 3 Shape", "", 0.0f, 5.0f, 0.0f, 1.0f, 1.0f},
     {"lfo3_sync", "LFO 3 Sync", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo3_division", "LFO 3 Division", "", 0.0f, 8.0f, 4.0f, 1.0f, 1.0f},
     {"lfo3_retrigger", "LFO 3 Retrigger", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {"lfo3_phase", "LFO 3 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo3_oneShot", "LFO 3 One shot", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo3_unipolar", "LFO 3 Unipolar", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo4_rate", "LFO 4 Rate", "Hz", 0.02f, 40.0f, 1.0f, 0.3f, 0.0f},
     {"lfo4_shape", "LFO 4 Shape", "", 0.0f, 5.0f, 0.0f, 1.0f, 1.0f},
     {"lfo4_sync", "LFO 4 Sync", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo4_division", "LFO 4 Division", "", 0.0f, 8.0f, 4.0f, 1.0f, 1.0f},
     {"lfo4_retrigger", "LFO 4 Retrigger", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {"lfo4_phase", "LFO 4 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo4_oneShot", "LFO 4 One shot", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo4_unipolar", "LFO 4 Unipolar", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo5_rate", "LFO 5 Rate", "Hz", 0.02f, 40.0f, 1.0f, 0.3f, 0.0f},
     {"lfo5_shape", "LFO 5 Shape", "", 0.0f, 5.0f, 0.0f, 1.0f, 1.0f},
     {"lfo5_sync", "LFO 5 Sync", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo5_division", "LFO 5 Division", "", 0.0f, 8.0f, 4.0f, 1.0f, 1.0f},
     {"lfo5_retrigger", "LFO 5 Retrigger", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {"lfo5_phase", "LFO 5 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo5_oneShot", "LFO 5 One shot", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo5_unipolar", "LFO 5 Unipolar", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo6_rate", "LFO 6 Rate", "Hz", 0.02f, 40.0f, 1.0f, 0.3f, 0.0f},
     {"lfo6_shape", "LFO 6 Shape", "", 0.0f, 5.0f, 0.0f, 1.0f, 1.0f},
     {"lfo6_sync", "LFO 6 Sync", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo6_division", "LFO 6 Division", "", 0.0f, 8.0f, 4.0f, 1.0f, 1.0f},
     {"lfo6_retrigger", "LFO 6 Retrigger", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {"lfo6_phase", "LFO 6 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo6_oneShot", "LFO 6 One shot", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo6_unipolar", "LFO 6 Unipolar", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo7_rate", "LFO 7 Rate", "Hz", 0.02f, 40.0f, 1.0f, 0.3f, 0.0f},
     {"lfo7_shape", "LFO 7 Shape", "", 0.0f, 5.0f, 0.0f, 1.0f, 1.0f},
     {"lfo7_sync", "LFO 7 Sync", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo7_division", "LFO 7 Division", "", 0.0f, 8.0f, 4.0f, 1.0f, 1.0f},
     {"lfo7_retrigger", "LFO 7 Retrigger", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {"lfo7_phase", "LFO 7 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo7_oneShot", "LFO 7 One shot", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo7_unipolar", "LFO 7 Unipolar", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo8_rate", "LFO 8 Rate", "Hz", 0.02f, 40.0f, 1.0f, 0.3f, 0.0f},
     {"lfo8_shape", "LFO 8 Shape", "", 0.0f, 5.0f, 0.0f, 1.0f, 1.0f},
     {"lfo8_sync", "LFO 8 Sync", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo8_division", "LFO 8 Division", "", 0.0f, 8.0f, 4.0f, 1.0f, 1.0f},
     {"lfo8_retrigger", "LFO 8 Retrigger", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {"lfo8_phase", "LFO 8 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo8_oneShot", "LFO 8 One shot", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo8_unipolar", "LFO 8 Unipolar", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo9_rate", "LFO 9 Rate", "Hz", 0.02f, 40.0f, 1.0f, 0.3f, 0.0f},
     {"lfo9_shape", "LFO 9 Shape", "", 0.0f, 5.0f, 0.0f, 1.0f, 1.0f},
     {"lfo9_sync", "LFO 9 Sync", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo9_division", "LFO 9 Division", "", 0.0f, 8.0f, 4.0f, 1.0f, 1.0f},
     {"lfo9_retrigger", "LFO 9 Retrigger", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {"lfo9_phase", "LFO 9 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo9_oneShot", "LFO 9 One shot", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo9_unipolar", "LFO 9 Unipolar", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo10_rate", "LFO 10 Rate", "Hz", 0.02f, 40.0f, 1.0f, 0.3f, 0.0f},
     {"lfo10_shape", "LFO 10 Shape", "", 0.0f, 5.0f, 0.0f, 1.0f, 1.0f},
     {"lfo10_sync", "LFO 10 Sync", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo10_division", "LFO 10 Division", "", 0.0f, 8.0f, 4.0f, 1.0f, 1.0f},
     {"lfo10_retrigger", "LFO 10 Retrigger", "", 0.0f, 1.0f, 1.0f, 1.0f, 1.0f},
     {"lfo10_phase", "LFO 10 Phase", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo10_oneShot", "LFO 10 One shot", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"lfo10_unipolar", "LFO 10 Unipolar", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"macro1", "Macro 1", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"macro2", "Macro 2", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"macro3", "Macro 3", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"macro4", "Macro 4", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"macro5", "Macro 5", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"macro6", "Macro 6", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"macro7", "Macro 7", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"macro8", "Macro 8", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod1_source", "Route 1 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod1_destination", "Route 1 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod1_amount", "Route 1 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod1_polarity", "Route 1 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod1_bypass", "Route 1 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod1_curve", "Route 1 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod2_source", "Route 2 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod2_destination", "Route 2 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod2_amount", "Route 2 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod2_polarity", "Route 2 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod2_bypass", "Route 2 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod2_curve", "Route 2 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod3_source", "Route 3 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod3_destination", "Route 3 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod3_amount", "Route 3 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod3_polarity", "Route 3 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod3_bypass", "Route 3 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod3_curve", "Route 3 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod4_source", "Route 4 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod4_destination", "Route 4 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod4_amount", "Route 4 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod4_polarity", "Route 4 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod4_bypass", "Route 4 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod4_curve", "Route 4 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod5_source", "Route 5 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod5_destination", "Route 5 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod5_amount", "Route 5 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod5_polarity", "Route 5 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod5_bypass", "Route 5 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod5_curve", "Route 5 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod6_source", "Route 6 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod6_destination", "Route 6 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod6_amount", "Route 6 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod6_polarity", "Route 6 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod6_bypass", "Route 6 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod6_curve", "Route 6 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod7_source", "Route 7 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod7_destination", "Route 7 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod7_amount", "Route 7 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod7_polarity", "Route 7 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod7_bypass", "Route 7 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod7_curve", "Route 7 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod8_source", "Route 8 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod8_destination", "Route 8 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod8_amount", "Route 8 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod8_polarity", "Route 8 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod8_bypass", "Route 8 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod8_curve", "Route 8 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod9_source", "Route 9 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod9_destination", "Route 9 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod9_amount", "Route 9 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod9_polarity", "Route 9 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod9_bypass", "Route 9 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod9_curve", "Route 9 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod10_source", "Route 10 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod10_destination", "Route 10 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod10_amount", "Route 10 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod10_polarity", "Route 10 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod10_bypass", "Route 10 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod10_curve", "Route 10 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod11_source", "Route 11 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod11_destination", "Route 11 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod11_amount", "Route 11 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod11_polarity", "Route 11 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod11_bypass", "Route 11 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod11_curve", "Route 11 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod12_source", "Route 12 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod12_destination", "Route 12 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod12_amount", "Route 12 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod12_polarity", "Route 12 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod12_bypass", "Route 12 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod12_curve", "Route 12 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod13_source", "Route 13 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod13_destination", "Route 13 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod13_amount", "Route 13 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod13_polarity", "Route 13 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod13_bypass", "Route 13 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod13_curve", "Route 13 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod14_source", "Route 14 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod14_destination", "Route 14 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod14_amount", "Route 14 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod14_polarity", "Route 14 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod14_bypass", "Route 14 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod14_curve", "Route 14 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod15_source", "Route 15 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod15_destination", "Route 15 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod15_amount", "Route 15 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod15_polarity", "Route 15 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod15_bypass", "Route 15 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod15_curve", "Route 15 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod16_source", "Route 16 Source", "", 0.0f, 29.0f, 0.0f, 1.0f, 1.0f},
     {"mod16_destination", "Route 16 Destination", "", 0.0f, 21.0f, 0.0f, 1.0f, 1.0f},
     {"mod16_amount", "Route 16 Amount", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"mod16_polarity", "Route 16 Unipolar source", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod16_bypass", "Route 16 Bypass", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mod16_curve", "Route 16 Curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"sat_enabled", "sat enabled", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"sat_drive", "sat Drive", "dB", 0.0f, 30.0f, 6.0f, 1.0f, 0.0f},
     {"sat_mix", "sat Mix", "", 0.0f, 1.0f, 0.5f, 1.0f, 0.0f},
     {"chorus_enabled", "chorus enabled", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"chorus_rate", "chorus Rate", "Hz", 0.05f, 5.0f, 0.3f, 1.0f, 0.0f},
     {"chorus_depth", "chorus Depth", "", 0.0f, 1.0f, 0.4f, 1.0f, 0.0f},
     {"chorus_mix", "chorus Mix", "", 0.0f, 1.0f, 0.3f, 1.0f, 0.0f},
     {"phaser_enabled", "phaser enabled", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"phaser_rate", "phaser Rate", "Hz", 0.05f, 5.0f, 0.2f, 1.0f, 0.0f},
     {"phaser_depth", "phaser Depth", "", 0.0f, 1.0f, 0.5f, 1.0f, 0.0f},
     {"phaser_mix", "phaser Mix", "", 0.0f, 1.0f, 0.35f, 1.0f, 0.0f},
     {"delay_enabled", "delay enabled", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"delay_time", "delay Time", "s", 0.01f, 2.0f, 0.3f, 1.0f, 0.0f},
     {"delay_feedback", "delay Feedback", "", 0.0f, 0.92f, 0.35f, 1.0f, 0.0f},
     {"delay_mix", "delay Mix", "", 0.0f, 1.0f, 0.25f, 1.0f, 0.0f},
     {"delay_sync", "delay Sync", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"delay_division", "delay Division", "", 0.0f, 8.0f, 4.0f, 1.0f, 1.0f},
     {"reverb_enabled", "reverb enabled", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"reverb_size", "reverb Size", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"reverb_damping", "reverb Damping", "", 0.0f, 1.0f, 0.5f, 1.0f, 0.0f},
     {"reverb_mix", "reverb Mix", "", 0.0f, 1.0f, 0.25f, 1.0f, 0.0f},
     {"eq_enabled", "eq enabled", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"eq_low", "eq Low", "dB", -18.0f, 18.0f, 0.0f, 1.0f, 0.0f},
     {"eq_mid", "eq Mid", "dB", -18.0f, 18.0f, 0.0f, 1.0f, 0.0f},
     {"eq_high", "eq High", "dB", -18.0f, 18.0f, 0.0f, 1.0f, 0.0f},
     {"eq_frequency", "eq Mid frequency", "Hz", 200.0f, 6000.0f, 1200.0f, 0.4f, 0.0f},
     {"comp_enabled", "comp enabled", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"comp_threshold", "comp Threshold", "dB", -48.0f, 0.0f, -18.0f, 1.0f, 0.0f},
     {"comp_ratio", "comp Ratio", "", 1.0f, 16.0f, 4.0f, 1.0f, 0.0f},
     {"comp_attack", "comp Attack", "ms", 0.1f, 100.0f, 10.0f, 0.4f, 0.0f},
     {"comp_release", "comp Release", "ms", 10.0f, 1000.0f, 150.0f, 0.4f, 0.0f},
     {"comp_mix", "comp Mix", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"limiter_enabled", "limiter enabled", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"limiter_ceiling", "limiter Ceiling", "dB", -12.0f, 0.0f, -1.0f, 1.0f, 0.0f},
     {"fx_slot0", "FX slot 1", "", 0.0f, 7.0f, 0.0f, 1.0f, 1.0f},
     {"fx_slot1", "FX slot 2", "", 0.0f, 7.0f, 1.0f, 1.0f, 1.0f},
     {"fx_slot2", "FX slot 3", "", 0.0f, 7.0f, 2.0f, 1.0f, 1.0f},
     {"fx_slot3", "FX slot 4", "", 0.0f, 7.0f, 3.0f, 1.0f, 1.0f},
     {"fx_slot4", "FX slot 5", "", 0.0f, 7.0f, 4.0f, 1.0f, 1.0f},
     {"fx_slot5", "FX slot 6", "", 0.0f, 7.0f, 5.0f, 1.0f, 1.0f},
     {"fx_slot6", "FX slot 7", "", 0.0f, 7.0f, 6.0f, 1.0f, 1.0f},
     {"fx_slot7", "FX slot 8", "", 0.0f, 7.0f, 7.0f, 1.0f, 1.0f},
     {"hybrid1_mode", "Osc 1 Engine", "", 0.0f, 4.0f, 0.0f, 1.0f, 1.0f},
     {"hybrid1_root", "Osc 1 Root note", "MIDI", 0.0f, 127.0f, 60.0f, 1.0f, 1.0f},
     {"hybrid1_start", "Osc 1 Start", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid1_end", "Osc 1 End", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"hybrid1_reverse", "Osc 1 Reverse", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"hybrid1_loop", "Osc 1 Loop", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"hybrid1_crossfade", "Osc 1 Loop crossfade", "s", 0.0f, 0.2f, 0.01f, 1.0f, 0.0f},
     {"hybrid1_sampleGain", "Osc 1 Sample gain", "", 0.0f, 2.0f, 1.0f, 1.0f, 0.0f},
     {"hybrid1_grainPosition", "Osc 1 Grain position", "", 0.0f, 1.0f, 0.25f, 1.0f, 0.0f},
     {"hybrid1_grainSize", "Osc 1 Grain size", "s", 0.005f, 1.0f, 0.1f, 0.4f, 0.0f},
     {"hybrid1_grainDensity", "Osc 1 Grain density", "/s", 1.0f, 100.0f, 20.0f, 1.0f, 0.0f},
     {"hybrid1_grainPitch", "Osc 1 Grain pitch", "st", -24.0f, 24.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid1_grainSpread", "Osc 1 Grain spread", "", 0.0f, 1.0f, 0.5f, 1.0f, 0.0f},
     {"hybrid1_grainJitter", "Osc 1 Position jitter", "", 0.0f, 1.0f, 0.1f, 1.0f, 0.0f},
     {"hybrid1_grainMotion", "Osc 1 Source motion", "x", -2.0f, 2.0f, 1.0f, 1.0f, 0.0f},
     {"hybrid1_spectralPosition", "Osc 1 Spectral time", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid1_spectralTilt", "Osc 1 Spectral tilt", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid1_spectralHarmonics", "Osc 1 Harmonics", "", 1.0f, 32.0f, 32.0f, 1.0f, 1.0f},
     {"hybrid2_mode", "Osc 2 Engine", "", 0.0f, 4.0f, 0.0f, 1.0f, 1.0f},
     {"hybrid2_root", "Osc 2 Root note", "MIDI", 0.0f, 127.0f, 60.0f, 1.0f, 1.0f},
     {"hybrid2_start", "Osc 2 Start", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid2_end", "Osc 2 End", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"hybrid2_reverse", "Osc 2 Reverse", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"hybrid2_loop", "Osc 2 Loop", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"hybrid2_crossfade", "Osc 2 Loop crossfade", "s", 0.0f, 0.2f, 0.01f, 1.0f, 0.0f},
     {"hybrid2_sampleGain", "Osc 2 Sample gain", "", 0.0f, 2.0f, 1.0f, 1.0f, 0.0f},
     {"hybrid2_grainPosition", "Osc 2 Grain position", "", 0.0f, 1.0f, 0.25f, 1.0f, 0.0f},
     {"hybrid2_grainSize", "Osc 2 Grain size", "s", 0.005f, 1.0f, 0.1f, 0.4f, 0.0f},
     {"hybrid2_grainDensity", "Osc 2 Grain density", "/s", 1.0f, 100.0f, 20.0f, 1.0f, 0.0f},
     {"hybrid2_grainPitch", "Osc 2 Grain pitch", "st", -24.0f, 24.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid2_grainSpread", "Osc 2 Grain spread", "", 0.0f, 1.0f, 0.5f, 1.0f, 0.0f},
     {"hybrid2_grainJitter", "Osc 2 Position jitter", "", 0.0f, 1.0f, 0.1f, 1.0f, 0.0f},
     {"hybrid2_grainMotion", "Osc 2 Source motion", "x", -2.0f, 2.0f, 1.0f, 1.0f, 0.0f},
     {"hybrid2_spectralPosition", "Osc 2 Spectral time", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid2_spectralTilt", "Osc 2 Spectral tilt", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid2_spectralHarmonics", "Osc 2 Harmonics", "", 1.0f, 32.0f, 32.0f, 1.0f, 1.0f},
     {"hybrid3_mode", "Osc 3 Engine", "", 0.0f, 4.0f, 0.0f, 1.0f, 1.0f},
     {"hybrid3_root", "Osc 3 Root note", "MIDI", 0.0f, 127.0f, 60.0f, 1.0f, 1.0f},
     {"hybrid3_start", "Osc 3 Start", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid3_end", "Osc 3 End", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"hybrid3_reverse", "Osc 3 Reverse", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"hybrid3_loop", "Osc 3 Loop", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"hybrid3_crossfade", "Osc 3 Loop crossfade", "s", 0.0f, 0.2f, 0.01f, 1.0f, 0.0f},
     {"hybrid3_sampleGain", "Osc 3 Sample gain", "", 0.0f, 2.0f, 1.0f, 1.0f, 0.0f},
     {"hybrid3_grainPosition", "Osc 3 Grain position", "", 0.0f, 1.0f, 0.25f, 1.0f, 0.0f},
     {"hybrid3_grainSize", "Osc 3 Grain size", "s", 0.005f, 1.0f, 0.1f, 0.4f, 0.0f},
     {"hybrid3_grainDensity", "Osc 3 Grain density", "/s", 1.0f, 100.0f, 20.0f, 1.0f, 0.0f},
     {"hybrid3_grainPitch", "Osc 3 Grain pitch", "st", -24.0f, 24.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid3_grainSpread", "Osc 3 Grain spread", "", 0.0f, 1.0f, 0.5f, 1.0f, 0.0f},
     {"hybrid3_grainJitter", "Osc 3 Position jitter", "", 0.0f, 1.0f, 0.1f, 1.0f, 0.0f},
     {"hybrid3_grainMotion", "Osc 3 Source motion", "x", -2.0f, 2.0f, 1.0f, 1.0f, 0.0f},
     {"hybrid3_spectralPosition", "Osc 3 Spectral time", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid3_spectralTilt", "Osc 3 Spectral tilt", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid3_spectralHarmonics", "Osc 3 Harmonics", "", 1.0f, 32.0f, 32.0f, 1.0f, 1.0f},
     {"sub_route", "Sub routing", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"noise_route", "Noise routing", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"mpe", "MPE", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"mpeRange", "MPE bend range", "st", 1.0f, 96.0f, 48.0f, 1.0f, 1.0f},
     {"internalTempo", "Internal tempo", "BPM", 20.0f, 300.0f, 120.0f, 1.0f, 0.0f},
     {"arp_enabled", "Arpeggiator", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"arp_order", "Arp order", "", 0.0f, 4.0f, 0.0f, 1.0f, 1.0f},
     {"arp_octaves", "Octaves", "", 1.0f, 4.0f, 1.0f, 1.0f, 1.0f},
     {"arp_division", "Arp division", "", 0.0f, 8.0f, 3.0f, 1.0f, 1.0f},
     {"arp_gate", "Arp gate", "", 0.05f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"arp_swing", "Arp swing", "", 0.0f, 0.45f, 0.0f, 1.0f, 0.0f},
     {"arp_latch", "Arp latch", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"seq_enabled", "Sequencer", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"seq_length", "Pattern length", "", 1.0f, 16.0f, 16.0f, 1.0f, 1.0f},
     {"seq_division", "Step division", "", 0.0f, 8.0f, 2.0f, 1.0f, 1.0f},
     {"seq_swing", "Step swing", "", 0.0f, 0.45f, 0.0f, 1.0f, 0.0f},
     {"seq_transpose", "Pattern transpose", "st", -24.0f, 24.0f, 0.0f, 1.0f, 1.0f},
     {"seq_latch", "Pattern latch", "", 0.0f, 1.0f, 0.0f, 1.0f, 1.0f},
     {"step1_note", "Step 1 note", "st", -24.0f, 24.0f, 0.0f, 1.0f, 1.0f},
     {"step1_velocity", "Step 1 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step1_gate", "Step 1 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step1_probability", "Step 1 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step2_note", "Step 2 note", "st", -24.0f, 24.0f, 7.0f, 1.0f, 1.0f},
     {"step2_velocity", "Step 2 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step2_gate", "Step 2 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step2_probability", "Step 2 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step3_note", "Step 3 note", "st", -24.0f, 24.0f, 12.0f, 1.0f, 1.0f},
     {"step3_velocity", "Step 3 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step3_gate", "Step 3 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step3_probability", "Step 3 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step4_note", "Step 4 note", "st", -24.0f, 24.0f, 3.0f, 1.0f, 1.0f},
     {"step4_velocity", "Step 4 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step4_gate", "Step 4 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step4_probability", "Step 4 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step5_note", "Step 5 note", "st", -24.0f, 24.0f, 0.0f, 1.0f, 1.0f},
     {"step5_velocity", "Step 5 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step5_gate", "Step 5 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step5_probability", "Step 5 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step6_note", "Step 6 note", "st", -24.0f, 24.0f, 10.0f, 1.0f, 1.0f},
     {"step6_velocity", "Step 6 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step6_gate", "Step 6 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step6_probability", "Step 6 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step7_note", "Step 7 note", "st", -24.0f, 24.0f, 7.0f, 1.0f, 1.0f},
     {"step7_velocity", "Step 7 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step7_gate", "Step 7 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step7_probability", "Step 7 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step8_note", "Step 8 note", "st", -24.0f, 24.0f, 5.0f, 1.0f, 1.0f},
     {"step8_velocity", "Step 8 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step8_gate", "Step 8 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step8_probability", "Step 8 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step9_note", "Step 9 note", "st", -24.0f, 24.0f, 0.0f, 1.0f, 1.0f},
     {"step9_velocity", "Step 9 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step9_gate", "Step 9 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step9_probability", "Step 9 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step10_note", "Step 10 note", "st", -24.0f, 24.0f, 7.0f, 1.0f, 1.0f},
     {"step10_velocity", "Step 10 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step10_gate", "Step 10 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step10_probability", "Step 10 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step11_note", "Step 11 note", "st", -24.0f, 24.0f, 12.0f, 1.0f, 1.0f},
     {"step11_velocity", "Step 11 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step11_gate", "Step 11 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step11_probability", "Step 11 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step12_note", "Step 12 note", "st", -24.0f, 24.0f, 3.0f, 1.0f, 1.0f},
     {"step12_velocity", "Step 12 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step12_gate", "Step 12 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step12_probability", "Step 12 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step13_note", "Step 13 note", "st", -24.0f, 24.0f, 0.0f, 1.0f, 1.0f},
     {"step13_velocity", "Step 13 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step13_gate", "Step 13 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step13_probability", "Step 13 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step14_note", "Step 14 note", "st", -24.0f, 24.0f, 10.0f, 1.0f, 1.0f},
     {"step14_velocity", "Step 14 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step14_gate", "Step 14 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step14_probability", "Step 14 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step15_note", "Step 15 note", "st", -24.0f, 24.0f, 7.0f, 1.0f, 1.0f},
     {"step15_velocity", "Step 15 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step15_gate", "Step 15 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step15_probability", "Step 15 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"step16_note", "Step 16 note", "st", -24.0f, 24.0f, 5.0f, 1.0f, 1.0f},
     {"step16_velocity", "Step 16 velocity", "", 0.0f, 127.0f, 100.0f, 1.0f, 1.0f},
     {"step16_gate", "Step 16 gate", "", 0.0f, 1.0f, 0.6f, 1.0f, 0.0f},
     {"step16_probability", "Step 16 probability", "", 0.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"env1_attackCurve", "Env 1 attack curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"env1_decayCurve", "Env 1 decay curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"env1_releaseCurve", "Env 1 release curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"env2_attackCurve", "Env 2 attack curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"env2_decayCurve", "Env 2 decay curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"env2_releaseCurve", "Env 2 release curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"env3_attackCurve", "Env 3 attack curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"env3_decayCurve", "Env 3 decay curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"env3_releaseCurve", "Env 3 release curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"env4_attackCurve", "Env 4 attack curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"env4_decayCurve", "Env 4 decay curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"env4_releaseCurve", "Env 4 release curve", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo1_point1", "LFO 1 point 1", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo1_point2", "LFO 1 point 2", "", -1.0f, 1.0f, 0.7071067811865475f, 1.0f, 0.0f},
     {"lfo1_point3", "LFO 1 point 3", "", -1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"lfo1_point4", "LFO 1 point 4", "", -1.0f, 1.0f, 0.7071067811865476f, 1.0f, 0.0f},
     {"lfo1_point5", "LFO 1 point 5", "", -1.0f, 1.0f, 1.2246467991473532e-16f, 1.0f, 0.0f},
     {"lfo1_point6", "LFO 1 point 6", "", -1.0f, 1.0f, -0.7071067811865475f, 1.0f, 0.0f},
     {"lfo1_point7", "LFO 1 point 7", "", -1.0f, 1.0f, -1.0f, 1.0f, 0.0f},
     {"lfo1_point8", "LFO 1 point 8", "", -1.0f, 1.0f, -0.7071067811865477f, 1.0f, 0.0f},
     {"lfo2_point1", "LFO 2 point 1", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo2_point2", "LFO 2 point 2", "", -1.0f, 1.0f, 0.7071067811865475f, 1.0f, 0.0f},
     {"lfo2_point3", "LFO 2 point 3", "", -1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"lfo2_point4", "LFO 2 point 4", "", -1.0f, 1.0f, 0.7071067811865476f, 1.0f, 0.0f},
     {"lfo2_point5", "LFO 2 point 5", "", -1.0f, 1.0f, 1.2246467991473532e-16f, 1.0f, 0.0f},
     {"lfo2_point6", "LFO 2 point 6", "", -1.0f, 1.0f, -0.7071067811865475f, 1.0f, 0.0f},
     {"lfo2_point7", "LFO 2 point 7", "", -1.0f, 1.0f, -1.0f, 1.0f, 0.0f},
     {"lfo2_point8", "LFO 2 point 8", "", -1.0f, 1.0f, -0.7071067811865477f, 1.0f, 0.0f},
     {"lfo3_point1", "LFO 3 point 1", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo3_point2", "LFO 3 point 2", "", -1.0f, 1.0f, 0.7071067811865475f, 1.0f, 0.0f},
     {"lfo3_point3", "LFO 3 point 3", "", -1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"lfo3_point4", "LFO 3 point 4", "", -1.0f, 1.0f, 0.7071067811865476f, 1.0f, 0.0f},
     {"lfo3_point5", "LFO 3 point 5", "", -1.0f, 1.0f, 1.2246467991473532e-16f, 1.0f, 0.0f},
     {"lfo3_point6", "LFO 3 point 6", "", -1.0f, 1.0f, -0.7071067811865475f, 1.0f, 0.0f},
     {"lfo3_point7", "LFO 3 point 7", "", -1.0f, 1.0f, -1.0f, 1.0f, 0.0f},
     {"lfo3_point8", "LFO 3 point 8", "", -1.0f, 1.0f, -0.7071067811865477f, 1.0f, 0.0f},
     {"lfo4_point1", "LFO 4 point 1", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo4_point2", "LFO 4 point 2", "", -1.0f, 1.0f, 0.7071067811865475f, 1.0f, 0.0f},
     {"lfo4_point3", "LFO 4 point 3", "", -1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"lfo4_point4", "LFO 4 point 4", "", -1.0f, 1.0f, 0.7071067811865476f, 1.0f, 0.0f},
     {"lfo4_point5", "LFO 4 point 5", "", -1.0f, 1.0f, 1.2246467991473532e-16f, 1.0f, 0.0f},
     {"lfo4_point6", "LFO 4 point 6", "", -1.0f, 1.0f, -0.7071067811865475f, 1.0f, 0.0f},
     {"lfo4_point7", "LFO 4 point 7", "", -1.0f, 1.0f, -1.0f, 1.0f, 0.0f},
     {"lfo4_point8", "LFO 4 point 8", "", -1.0f, 1.0f, -0.7071067811865477f, 1.0f, 0.0f},
     {"lfo5_point1", "LFO 5 point 1", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo5_point2", "LFO 5 point 2", "", -1.0f, 1.0f, 0.7071067811865475f, 1.0f, 0.0f},
     {"lfo5_point3", "LFO 5 point 3", "", -1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"lfo5_point4", "LFO 5 point 4", "", -1.0f, 1.0f, 0.7071067811865476f, 1.0f, 0.0f},
     {"lfo5_point5", "LFO 5 point 5", "", -1.0f, 1.0f, 1.2246467991473532e-16f, 1.0f, 0.0f},
     {"lfo5_point6", "LFO 5 point 6", "", -1.0f, 1.0f, -0.7071067811865475f, 1.0f, 0.0f},
     {"lfo5_point7", "LFO 5 point 7", "", -1.0f, 1.0f, -1.0f, 1.0f, 0.0f},
     {"lfo5_point8", "LFO 5 point 8", "", -1.0f, 1.0f, -0.7071067811865477f, 1.0f, 0.0f},
     {"lfo6_point1", "LFO 6 point 1", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo6_point2", "LFO 6 point 2", "", -1.0f, 1.0f, 0.7071067811865475f, 1.0f, 0.0f},
     {"lfo6_point3", "LFO 6 point 3", "", -1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"lfo6_point4", "LFO 6 point 4", "", -1.0f, 1.0f, 0.7071067811865476f, 1.0f, 0.0f},
     {"lfo6_point5", "LFO 6 point 5", "", -1.0f, 1.0f, 1.2246467991473532e-16f, 1.0f, 0.0f},
     {"lfo6_point6", "LFO 6 point 6", "", -1.0f, 1.0f, -0.7071067811865475f, 1.0f, 0.0f},
     {"lfo6_point7", "LFO 6 point 7", "", -1.0f, 1.0f, -1.0f, 1.0f, 0.0f},
     {"lfo6_point8", "LFO 6 point 8", "", -1.0f, 1.0f, -0.7071067811865477f, 1.0f, 0.0f},
     {"lfo7_point1", "LFO 7 point 1", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo7_point2", "LFO 7 point 2", "", -1.0f, 1.0f, 0.7071067811865475f, 1.0f, 0.0f},
     {"lfo7_point3", "LFO 7 point 3", "", -1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"lfo7_point4", "LFO 7 point 4", "", -1.0f, 1.0f, 0.7071067811865476f, 1.0f, 0.0f},
     {"lfo7_point5", "LFO 7 point 5", "", -1.0f, 1.0f, 1.2246467991473532e-16f, 1.0f, 0.0f},
     {"lfo7_point6", "LFO 7 point 6", "", -1.0f, 1.0f, -0.7071067811865475f, 1.0f, 0.0f},
     {"lfo7_point7", "LFO 7 point 7", "", -1.0f, 1.0f, -1.0f, 1.0f, 0.0f},
     {"lfo7_point8", "LFO 7 point 8", "", -1.0f, 1.0f, -0.7071067811865477f, 1.0f, 0.0f},
     {"lfo8_point1", "LFO 8 point 1", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo8_point2", "LFO 8 point 2", "", -1.0f, 1.0f, 0.7071067811865475f, 1.0f, 0.0f},
     {"lfo8_point3", "LFO 8 point 3", "", -1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"lfo8_point4", "LFO 8 point 4", "", -1.0f, 1.0f, 0.7071067811865476f, 1.0f, 0.0f},
     {"lfo8_point5", "LFO 8 point 5", "", -1.0f, 1.0f, 1.2246467991473532e-16f, 1.0f, 0.0f},
     {"lfo8_point6", "LFO 8 point 6", "", -1.0f, 1.0f, -0.7071067811865475f, 1.0f, 0.0f},
     {"lfo8_point7", "LFO 8 point 7", "", -1.0f, 1.0f, -1.0f, 1.0f, 0.0f},
     {"lfo8_point8", "LFO 8 point 8", "", -1.0f, 1.0f, -0.7071067811865477f, 1.0f, 0.0f},
     {"lfo9_point1", "LFO 9 point 1", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo9_point2", "LFO 9 point 2", "", -1.0f, 1.0f, 0.7071067811865475f, 1.0f, 0.0f},
     {"lfo9_point3", "LFO 9 point 3", "", -1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"lfo9_point4", "LFO 9 point 4", "", -1.0f, 1.0f, 0.7071067811865476f, 1.0f, 0.0f},
     {"lfo9_point5", "LFO 9 point 5", "", -1.0f, 1.0f, 1.2246467991473532e-16f, 1.0f, 0.0f},
     {"lfo9_point6", "LFO 9 point 6", "", -1.0f, 1.0f, -0.7071067811865475f, 1.0f, 0.0f},
     {"lfo9_point7", "LFO 9 point 7", "", -1.0f, 1.0f, -1.0f, 1.0f, 0.0f},
     {"lfo9_point8", "LFO 9 point 8", "", -1.0f, 1.0f, -0.7071067811865477f, 1.0f, 0.0f},
     {"lfo10_point1", "LFO 10 point 1", "", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"lfo10_point2", "LFO 10 point 2", "", -1.0f, 1.0f, 0.7071067811865475f, 1.0f, 0.0f},
     {"lfo10_point3", "LFO 10 point 3", "", -1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
     {"lfo10_point4", "LFO 10 point 4", "", -1.0f, 1.0f, 0.7071067811865476f, 1.0f, 0.0f},
     {"lfo10_point5", "LFO 10 point 5", "", -1.0f, 1.0f, 1.2246467991473532e-16f, 1.0f, 0.0f},
     {"lfo10_point6", "LFO 10 point 6", "", -1.0f, 1.0f, -0.7071067811865475f, 1.0f, 0.0f},
     {"lfo10_point7", "LFO 10 point 7", "", -1.0f, 1.0f, -1.0f, 1.0f, 0.0f},
     {"lfo10_point8", "LFO 10 point 8", "", -1.0f, 1.0f, -0.7071067811865477f, 1.0f, 0.0f},
     {"osc1_warp2", "Osc 1 warp 2", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"osc1_warp2Amount", "Osc 1 warp 2 depth", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc2_warp2", "Osc 2 warp 2", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"osc2_warp2Amount", "Osc 2 warp 2 depth", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"osc3_warp2", "Osc 3 warp 2", "", 0.0f, 3.0f, 0.0f, 1.0f, 1.0f},
     {"osc3_warp2Amount", "Osc 3 warp 2 depth", "", 0.0f, 1.0f, 0.0f, 1.0f, 0.0f},
     {"hybrid1_oneShot", "Osc 1 one shot", "", 0, 1, 0, 1, 1},
     {"hybrid2_oneShot", "Osc 2 one shot", "", 0, 1, 0, 1, 1},
     {"hybrid3_oneShot", "Osc 3 one shot", "", 0, 1, 0, 1, 1}}};
inline constexpr std::array<const char *, 30> sourceNames{
    {"Off",         "Envelope 1", "Envelope 2",
     "Envelope 3",  "Envelope 4", "LFO 1",
     "LFO 2",       "LFO 3",      "LFO 4",
     "LFO 5",       "LFO 6",      "LFO 7",
     "LFO 8",       "LFO 9",      "LFO 10",
     "Velocity",    "Note",       "Random",
     "Mod wheel",   "Pressure",   "Release velocity",
     "Timbre CC74", "Macro 1",    "Macro 2",
     "Macro 3",     "Macro 4",    "Macro 5",
     "Macro 6",     "Macro 7",    "Macro 8"}};
inline constexpr std::array<const char *, 22> destinationNames{
    {"Off",        "Osc 1 table",     "Osc 1 pitch",        "Osc 1 level",     "Osc 1 pan",
     "Osc 1 warp", "Osc 2 table",     "Osc 2 pitch",        "Osc 2 level",     "Osc 2 pan",
     "Osc 2 warp", "Osc 3 table",     "Osc 3 pitch",        "Osc 3 level",     "Osc 3 pan",
     "Osc 3 warp", "Filter 1 cutoff", "Filter 1 resonance", "Filter 2 cutoff", "Filter 2 resonance",
     "Sub level",  "Noise level"}};
struct OscIndices
{
    int table, level, pitch, fine, pan, unison, detune, spread, phase, random, warp, warpAmount,
        interaction, interactionAmount, route;
};
inline constexpr std::array<OscIndices, 3> oscillatorIndices{{
    {timbre, osc1_level, osc1_pitch, osc1_fine, osc1_pan, osc1_unison, osc1_detune, osc1_spread,
     osc1_phase, osc1_random, osc1_warp, osc1_warpAmount, osc1_interaction, osc1_interactionAmount,
     osc1_route},
    {osc2_table, osc2_level, osc2_pitch, osc2_fine, osc2_pan, osc2_unison, osc2_detune, osc2_spread,
     osc2_phase, osc2_random, osc2_warp, osc2_warpAmount, osc2_interaction, osc2_interactionAmount,
     osc2_route},
    {osc3_table, osc3_level, osc3_pitch, osc3_fine, osc3_pan, osc3_unison, osc3_detune, osc3_spread,
     osc3_phase, osc3_random, osc3_warp, osc3_warpAmount, osc3_interaction, osc3_interactionAmount,
     osc3_route},
}};
inline constexpr std::array<int, 8> effectEnabled{sat_enabled,   chorus_enabled, phaser_enabled,
                                                  delay_enabled, reverb_enabled, eq_enabled,
                                                  comp_enabled,  limiter_enabled};
inline constexpr std::array<const char *, 8> effectNames{"Saturation",   "Chorus", "Phaser",
                                                         "Stereo delay", "Reverb", "Parametric EQ",
                                                         "Compressor",   "Limiter"};
struct Preset
{
    const char *name;
    const char *category;
    const char *description;
    std::array<float, paramCount> values;
    const char *tags = "original";
    const char *author = "Prism Forge";
    std::array<const char *, 4> macroNames{"Brightness", "Motion", "Colour", "Expression"};
};
inline std::array<float, paramCount> defaultValues()
{
    std::array<float, paramCount> v{};
    for (size_t i = 0; i < v.size(); ++i)
        v[i] = parameters[i].def;
    return v;
}
inline const std::array<Preset, 113> presets = []
{
    std::array<Preset, 113> p{};
    auto set = [&](int i, const char *name, const char *category, const char *desc,
                   std::array<float, 8> first)
    {
        auto v = defaultValues();
        for (size_t n = 0; n < 8; ++n)
            v[n] = first[n];
        p[size_t(i)] = {name, category, desc, v};
    };
    set(0, "Init", "Init", "Neutral morph / C2-C6",
        {0.6f, 0.01f, 0.2f, 0.7f, 0.3f, 8000.0f, 0.15f, 0.5f});
    set(1, "Foundation", "Bass", "Round bass / C1-C3",
        {0.0f, 0.005f, 0.3f, 0.8f, 0.12f, 1500.0f, 0.1f, 0.8f});
    set(2, "Copper Pluck", "Pluck", "Resonant pluck / C3-C6",
        {0.67f, 0.002f, 0.32f, 0.0f, 0.15f, 5000.0f, 0.5f, 0.7f});
    set(3, "Glass Thread", "Keys", "Triangle keys / C3-C7",
        {0.33f, 0.004f, 1.2f, 0.1f, 0.8f, 11000.0f, 0.1f, 0.7f});
    set(4, "Night Alloy", "Pad", "Dark slow pad / C2-C5",
        {0.8f, 1.4f, 1.0f, 0.65f, 2.2f, 1200.0f, 0.25f, 0.55f});
    set(5, "Signal Flare", "Lead", "Square lead / C3-C6",
        {1.0f, 0.012f, 0.15f, 0.8f, 0.2f, 7500.0f, 0.3f, 0.5f});
    set(6, "Soft Circuit", "Keys", "Mellow keys / C2-C6",
        {0.4f, 0.06f, 0.5f, 0.5f, 0.7f, 2600.0f, 0.2f, 0.65f});
    set(7, "Resonant Wire", "Bass", "Resonant saw / C1-C3",
        {0.7f, 0.005f, 0.4f, 0.5f, 0.18f, 650.0f, 0.75f, 0.6f});
    set(8, "Horizon", "Pad", "Harmonic swell / C2-C5",
        {0.55f, 2.4f, 1.5f, 0.8f, 3.0f, 6500.0f, 0.1f, 0.5f});
    set(9, "Twin Current", "Bass", "Detuned low saw / C1-C3",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(10, "Arc Seven", "Lead", "Stereo unison / C3-C6",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(11, "Silver Dust", "Pad", "Air and slow motion / C2-C5",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(12, "Hollow Pulse", "Bass", "Pulse warp / C1-C3",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(13, "Phase Lantern", "Lead", "Phase modulation / C3-C6",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(14, "Velvet Steps", "Keys", "Triangle with echo / C2-C6",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(15, "Orbital", "Pad", "LFO filter movement / C2-C5",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(16, "Ion Bell", "Keys", "Ring-modulated bell / C3-C6",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(17, "Carbon", "Bass", "Driven lowpass / C1-C3",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(18, "Wide Awake", "Lead", "Triple oscillator / C3-C6",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(19, "Rain Circuit", "Atmosphere", "Filtered noise / C2-C5",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(20, "Slow Bloom", "Pad", "Four-envelope movement / C2-C5",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(21, "Copper Hall", "Pluck", "Pluck in reverb / C3-C6",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(22, "Ribbon", "Lead", "Mono legato glide / C3-C6",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(23, "Cold Mirror", "Experimental", "Parallel notch and bandpass / C2-C5",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});
    set(24, "Pulse Relay", "Experimental", "LFO driven timbre / C2-C5",
        {.65f, .01f, .3f, .65f, .4f, 6500, .2f, .45f});

    p[9].values[osc2_level] = .7f;
    p[9].values[osc2_fine] = 9;
    p[9].values[cutoff] = 1200;
    p[10].values[osc1_unison] = 7;
    p[10].values[osc1_detune] = 22;
    p[10].values[chorus_enabled] = 1;
    p[11].values[attack] = 1.8f;
    p[11].values[release] = 3;
    p[11].values[noise_level] = .08f;
    p[11].values[osc2_level] = .4f;
    p[11].values[osc2_pitch] = 12;
    p[11].values[reverb_enabled] = 1;
    p[12].values[osc1_warp] = 1;
    p[12].values[osc1_warpAmount] = .6f;
    p[12].values[sub_level] = .3f;
    p[12].values[cutoff] = 1800;
    p[13].values[osc2_level] = .5f;
    p[13].values[osc2_interaction] = 1;
    p[13].values[osc2_interactionAmount] = .4f;
    p[13].values[osc2_pitch] = 12;
    p[14].values[timbre] = .3f;
    p[14].values[decay] = .8f;
    p[14].values[sustain] = .2f;
    p[14].values[delay_enabled] = 1;
    p[15].values[attack] = .8f;
    p[15].values[cutoff] = 1800;
    p[15].values[mod5_source] = 5;
    p[15].values[mod5_destination] = 16;
    p[15].values[mod5_amount] = .25f;
    p[15].values[lfo1_rate] = .3f;
    p[16].values[timbre] = 0;
    p[16].values[osc2_table] = 0;
    p[16].values[osc2_level] = .7f;
    p[16].values[osc2_pitch] = 19;
    p[16].values[osc2_interaction] = 2;
    p[16].values[osc2_interactionAmount] = 1;
    p[16].values[decay] = 1;
    p[16].values[sustain] = 0;
    p[17].values[cutoff] = 700;
    p[17].values[filter1_drive] = 14;
    p[17].values[sat_enabled] = 1;
    p[17].values[sub_level] = .4f;
    p[18].values[osc1_unison] = 3;
    p[18].values[osc2_level] = .5f;
    p[18].values[osc2_pitch] = 12;
    p[18].values[osc3_level] = .3f;
    p[18].values[osc3_pitch] = -12;
    p[18].values[delay_enabled] = 1;
    p[19].values[osc1_level] = .15f;
    p[19].values[noise_level] = .8f;
    p[19].values[noise_colour] = 1;
    p[19].values[filter1_type] = 2;
    p[19].values[cutoff] = 1400;
    p[19].values[attack] = 1;
    p[19].values[release] = 2;
    p[19].values[reverb_enabled] = 1;
    p[20].values[attack] = 2;
    p[20].values[release] = 3;
    p[20].values[mod5_source] = 2;
    p[20].values[mod5_destination] = 1;
    p[20].values[mod5_amount] = .4f;
    p[20].values[mod6_source] = 3;
    p[20].values[mod6_destination] = 16;
    p[20].values[mod6_amount] = .2f;
    p[20].values[env2_attack] = 3;
    p[20].values[env3_attack] = 2;
    p[21].values[sustain] = 0;
    p[21].values[decay] = .3f;
    p[21].values[reverb_enabled] = 1;
    p[21].values[reverb_size] = .85f;
    p[22].values[mono] = 1;
    p[22].values[glide] = .12f;
    p[22].values[osc1_unison] = 3;
    p[22].values[delay_enabled] = 1;
    p[23].values[filter2_enabled] = 1;
    p[23].values[filter_path] = 1;
    p[23].values[filter_mix] = .5f;
    p[23].values[filter1_type] = 3;
    p[23].values[filter2_type] = 2;
    p[23].values[filter2_cutoff] = 1800;
    p[23].values[phaser_enabled] = 1;
    p[23].values[mod5_source] = 5;
    p[23].values[mod5_destination] = 1;
    p[23].values[mod5_amount] = .4f;

    set(25, "Deep Anchor", "Bass", "Original bass / C1-C3",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[25].values[timbre] = 0.0f;
    p[25].values[sub_level] = 0.5f;
    p[25].values[cutoff] = 500.0f;
    set(26, "Reese Vector", "Bass", "Original bass / C1-C3",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[26].values[osc1_unison] = 3.0f;
    p[26].values[osc2_level] = 0.6f;
    p[26].values[osc2_fine] = -12.0f;
    p[26].values[cutoff] = 1800.0f;
    set(27, "Acid Etch", "Bass", "Original bass / C1-C3",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[27].values[cutoff] = 500.0f;
    p[27].values[resonance] = 0.8f;
    p[27].values[mod5_source] = 2.0f;
    p[27].values[mod5_destination] = 16.0f;
    p[27].values[mod5_amount] = 0.4f;
    p[27].values[env2_sustain] = 0.0f;
    set(28, "FM Ember", "Bass", "Original bass / C1-C3",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[28].values[osc2_level] = 0.6f;
    p[28].values[osc2_table] = 0.0f;
    p[28].values[osc2_interaction] = 1.0f;
    p[28].values[osc2_interactionAmount] = 0.35f;
    p[28].values[cutoff] = 2200.0f;
    set(29, "Gravel Growl", "Bass", "Original bass / C1-C3",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[29].values[osc1_warp] = 2.0f;
    p[29].values[osc1_warpAmount] = 0.8f;
    p[29].values[filter1_drive] = 12.0f;
    p[29].values[cutoff] = 800.0f;
    p[29].values[sub_level] = 0.2f;
    set(30, "Short Circuit", "Bass", "Original bass / C1-C3",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[30].values[sustain] = 0.0f;
    p[30].values[decay] = 0.12f;
    p[30].values[cutoff] = 1800.0f;
    p[30].values[release] = 0.05f;
    set(31, "Iron Floor", "Bass", "Original bass / C1-C3",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[31].values[sat_enabled] = 1.0f;
    p[31].values[sat_drive] = 18.0f;
    p[31].values[cutoff] = 900.0f;
    p[31].values[noise_level] = 0.04f;
    set(32, "Mono Prism", "Lead", "Original expressive lead / C3-C6",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[32].values[mono] = 1.0f;
    p[32].values[glide] = 0.08f;
    p[32].values[osc2_level] = 0.3f;
    p[32].values[osc2_pitch] = 12.0f;
    set(33, "Solar Stack", "Lead", "Original expressive lead / C3-C6",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[33].values[osc1_unison] = 5.0f;
    p[33].values[osc1_detune] = 25.0f;
    p[33].values[osc2_level] = 0.3f;
    p[33].values[chorus_enabled] = 1.0f;
    set(34, "Hard Horizon", "Lead", "Original expressive lead / C3-C6",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[34].values[osc1_warp] = 3.0f;
    p[34].values[osc1_warpAmount] = 0.5f;
    p[34].values[delay_enabled] = 1.0f;
    set(35, "Pressure Line", "Lead", "Original expressive lead / C3-C6",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[35].values[timbre] = 0.2f;
    p[35].values[mod5_source] = 19.0f;
    p[35].values[mod5_destination] = 16.0f;
    p[35].values[mod5_amount] = 0.3f;
    set(36, "Clean Vector", "Lead", "Original expressive lead / C3-C6",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[36].values[timbre] = 0.15f;
    p[36].values[cutoff] = 13000.0f;
    p[36].values[delay_enabled] = 1.0f;
    p[36].values[delay_mix] = 0.16f;
    set(37, "Warm Lattice", "Pad", "Original pad texture / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[37].values[attack] = 1.2f;
    p[37].values[release] = 3.0f;
    p[37].values[osc1_unison] = 3.0f;
    p[37].values[cutoff] = 2200.0f;
    p[37].values[reverb_enabled] = 1.0f;
    set(38, "Evolving Glass", "Pad", "Original pad texture / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[38].values[attack] = 1.0f;
    p[38].values[release] = 3.0f;
    p[38].values[mod5_source] = 5.0f;
    p[38].values[mod5_destination] = 1.0f;
    p[38].values[mod5_amount] = 0.3f;
    p[38].values[lfo1_rate] = 0.1f;
    p[38].values[reverb_enabled] = 1.0f;
    set(39, "Air Columns", "Pad", "Original pad texture / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[39].values[attack] = 2.0f;
    p[39].values[release] = 3.0f;
    p[39].values[noise_level] = 0.15f;
    p[39].values[filter1_type] = 1.0f;
    p[39].values[cutoff] = 500.0f;
    p[39].values[chorus_enabled] = 1.0f;
    set(40, "Dark Chamber", "Pad", "Original pad texture / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[40].values[attack] = 1.0f;
    p[40].values[release] = 4.0f;
    p[40].values[cutoff] = 600.0f;
    p[40].values[sub_level] = 0.15f;
    p[40].values[reverb_enabled] = 1.0f;
    set(41, "Grain Aurora", "Pad", "Original pad texture / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[41].values[hybrid1_mode] = 3.0f;
    p[41].values[attack] = 1.0f;
    p[41].values[release] = 3.0f;
    p[41].values[hybrid1_grainDensity] = 35.0f;
    p[41].values[hybrid1_grainSize] = 0.25f;
    p[41].values[reverb_enabled] = 1.0f;
    set(42, "Mallet Photon", "Keys", "Generated sample mallet / C3-C6",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[42].values[hybrid1_mode] = 1.0f;
    p[42].values[sustain] = 0.0f;
    p[42].values[decay] = 1.2f;
    p[42].values[release] = 0.6f;
    p[42].values[reverb_enabled] = 1.0f;
    set(43, "Electric Fold", "Keys", "Key-zoned original electric tone / C2-C6",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[43].values[hybrid1_mode] = 2.0f;
    p[43].values[decay] = 1.0f;
    p[43].values[sustain] = 0.25f;
    p[43].values[chorus_enabled] = 1.0f;
    set(44, "Clockwork", "Arp / Sequence", "Host-synced pattern / hold C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[44].values[arp_enabled] = 1.0f;
    p[44].values[arp_order] = 0.0f;
    p[44].values[arp_octaves] = 1.0f;
    p[44].values[arp_division] = 2.0f;
    p[44].values[arp_gate] = 0.3f;
    p[44].values[cutoff] = 1800.0f;
    p[44].values[delay_enabled] = 1.0f;
    p[44].values[delay_sync] = 1.0f;
    p[44].values[sustain] = 0.3f;
    p[44].values[decay] = 0.15f;
    set(45, "Octave Rails", "Arp / Sequence", "Host-synced pattern / hold C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[45].values[arp_enabled] = 1.0f;
    p[45].values[arp_order] = 1.0f;
    p[45].values[arp_octaves] = 2.0f;
    p[45].values[arp_division] = 3.0f;
    p[45].values[arp_gate] = 0.35f;
    p[45].values[cutoff] = 2300.0f;
    p[45].values[delay_enabled] = 1.0f;
    p[45].values[delay_sync] = 1.0f;
    p[45].values[sustain] = 0.3f;
    p[45].values[decay] = 0.15f;
    set(46, "Swing Alloy", "Arp / Sequence", "Host-synced pattern / hold C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[46].values[arp_enabled] = 1.0f;
    p[46].values[arp_order] = 2.0f;
    p[46].values[arp_octaves] = 3.0f;
    p[46].values[arp_division] = 2.0f;
    p[46].values[arp_gate] = 0.4f;
    p[46].values[cutoff] = 2800.0f;
    p[46].values[delay_enabled] = 1.0f;
    p[46].values[delay_sync] = 1.0f;
    p[46].values[sustain] = 0.3f;
    p[46].values[decay] = 0.15f;
    set(47, "Ascending Grid", "Arp / Sequence", "Host-synced pattern / hold C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[47].values[arp_enabled] = 1.0f;
    p[47].values[arp_order] = 3.0f;
    p[47].values[arp_octaves] = 1.0f;
    p[47].values[arp_division] = 3.0f;
    p[47].values[arp_gate] = 0.45f;
    p[47].values[cutoff] = 3300.0f;
    p[47].values[delay_enabled] = 1.0f;
    p[47].values[delay_sync] = 1.0f;
    p[47].values[sustain] = 0.3f;
    p[47].values[decay] = 0.15f;
    set(48, "Probability Lace", "Arp / Sequence", "Host-synced pattern / hold C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[48].values[arp_enabled] = 0.0f;
    p[48].values[arp_order] = 4.0f;
    p[48].values[arp_octaves] = 2.0f;
    p[48].values[arp_division] = 2.0f;
    p[48].values[arp_gate] = 0.5f;
    p[48].values[cutoff] = 3800.0f;
    p[48].values[delay_enabled] = 1.0f;
    p[48].values[delay_sync] = 1.0f;
    p[48].values[sustain] = 0.3f;
    p[48].values[decay] = 0.15f;
    p[48].values[seq_enabled] = 1.0f;
    p[48].values[seq_length] = 9.0f;
    p[48].values[seq_swing] = 0.0f;
    p[48].values[step3_probability] = 0.7f;
    p[48].values[step4_note] = 3.0f;
    p[48].values[step8_note] = 10.0f;
    set(49, "Minor Current", "Arp / Sequence", "Host-synced pattern / hold C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[49].values[arp_enabled] = 0.0f;
    p[49].values[arp_order] = 0.0f;
    p[49].values[arp_octaves] = 3.0f;
    p[49].values[arp_division] = 3.0f;
    p[49].values[arp_gate] = 0.55f;
    p[49].values[cutoff] = 4300.0f;
    p[49].values[delay_enabled] = 1.0f;
    p[49].values[delay_sync] = 1.0f;
    p[49].values[sustain] = 0.3f;
    p[49].values[decay] = 0.15f;
    p[49].values[seq_enabled] = 1.0f;
    p[49].values[seq_length] = 10.0f;
    p[49].values[seq_swing] = 0.15f;
    p[49].values[step3_probability] = 1.0f;
    p[49].values[step4_note] = 3.0f;
    p[49].values[step8_note] = 10.0f;
    set(50, "Latch Relay", "Arp / Sequence", "Host-synced pattern / hold C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[50].values[arp_enabled] = 0.0f;
    p[50].values[arp_order] = 1.0f;
    p[50].values[arp_octaves] = 1.0f;
    p[50].values[arp_division] = 2.0f;
    p[50].values[arp_gate] = 0.6000000000000001f;
    p[50].values[cutoff] = 4800.0f;
    p[50].values[delay_enabled] = 1.0f;
    p[50].values[delay_sync] = 1.0f;
    p[50].values[sustain] = 0.3f;
    p[50].values[decay] = 0.15f;
    p[50].values[seq_enabled] = 1.0f;
    p[50].values[seq_length] = 8.0f;
    p[50].values[seq_swing] = 0.0f;
    p[50].values[step3_probability] = 1.0f;
    p[50].values[step4_note] = 3.0f;
    p[50].values[step8_note] = 10.0f;
    set(51, "Pulse Geometry", "Arp / Sequence", "Host-synced pattern / hold C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[51].values[arp_enabled] = 0.0f;
    p[51].values[arp_order] = 2.0f;
    p[51].values[arp_octaves] = 2.0f;
    p[51].values[arp_division] = 3.0f;
    p[51].values[arp_gate] = 0.65f;
    p[51].values[cutoff] = 5300.0f;
    p[51].values[delay_enabled] = 1.0f;
    p[51].values[delay_sync] = 1.0f;
    p[51].values[sustain] = 0.3f;
    p[51].values[decay] = 0.15f;
    p[51].values[seq_enabled] = 1.0f;
    p[51].values[seq_length] = 9.0f;
    p[51].values[seq_swing] = 0.15f;
    p[51].values[step3_probability] = 1.0f;
    p[51].values[step4_note] = 3.0f;
    p[51].values[step8_note] = 10.0f;
    set(52, "Lift Engine", "Atmosphere", "Original effect / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[52].values[noise_level] = 0.6f;
    p[52].values[osc1_level] = 0.2f;
    p[52].values[attack] = 2.0f;
    p[52].values[mod5_source] = 2.0f;
    p[52].values[mod5_destination] = 16.0f;
    p[52].values[mod5_amount] = 0.7f;
    p[52].values[cutoff] = 200.0f;
    p[52].values[env2_attack] = 4.0f;
    p[52].values[env2_sustain] = 1.0f;
    set(53, "Falling Debris", "Atmosphere", "Original effect / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[53].values[noise_level] = 0.7f;
    p[53].values[sustain] = 0.0f;
    p[53].values[decay] = 1.5f;
    p[53].values[mod5_source] = 2.0f;
    p[53].values[mod5_destination] = 16.0f;
    p[53].values[mod5_amount] = 0.3f;
    p[53].values[env2_sustain] = 0.0f;
    set(54, "Subspace Drone", "Atmosphere", "Original effect / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[54].values[sub_level] = 0.5f;
    p[54].values[osc2_level] = 0.3f;
    p[54].values[osc2_pitch] = 7.0f;
    p[54].values[attack] = 1.0f;
    p[54].values[release] = 4.0f;
    p[54].values[cutoff] = 700.0f;
    set(55, "Dust Motion", "Atmosphere", "Original effect / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[55].values[noise_level] = 1.0f;
    p[55].values[osc1_level] = 0.0f;
    p[55].values[filter1_type] = 2.0f;
    p[55].values[cutoff] = 2200.0f;
    p[55].values[mod5_source] = 5.0f;
    p[55].values[mod5_destination] = 16.0f;
    p[55].values[mod5_amount] = 0.3f;
    p[55].values[lfo1_rate] = 0.2f;
    set(56, "Spectral Mist", "Atmosphere", "Original effect / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[56].values[hybrid1_mode] = 4.0f;
    p[56].values[hybrid1_spectralTilt] = 0.5f;
    p[56].values[attack] = 1.0f;
    p[56].values[release] = 3.0f;
    p[56].values[reverb_enabled] = 1.0f;
    set(57, "Grain Impact", "Atmosphere", "Original effect / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[57].values[hybrid1_mode] = 3.0f;
    p[57].values[hybrid1_grainDensity] = 80.0f;
    p[57].values[hybrid1_grainSize] = 0.035f;
    p[57].values[sustain] = 0.0f;
    p[57].values[decay] = 1.5f;
    p[57].values[reverb_enabled] = 1.0f;
    set(58, "Metal Passage", "Atmosphere", "Original effect / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[58].values[osc2_level] = 0.8f;
    p[58].values[osc2_interaction] = 2.0f;
    p[58].values[osc2_interactionAmount] = 1.0f;
    p[58].values[osc2_pitch] = 7.0f;
    p[58].values[attack] = 0.1f;
    p[58].values[release] = 2.0f;
    p[58].values[phaser_enabled] = 1.0f;
    set(59, "Spectral Loom", "Experimental", "Original engine study / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[59].values[hybrid1_mode] = 4.0f;
    p[59].values[hybrid1_spectralPosition] = 0.45f;
    p[59].values[hybrid1_spectralTilt] = -0.5f;
    p[59].values[attack] = 0.2f;
    p[59].values[release] = 1.5f;
    set(60, "Grain Parallax", "Experimental", "Original engine study / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[60].values[hybrid1_mode] = 3.0f;
    p[60].values[hybrid1_grainMotion] = -0.7f;
    p[60].values[hybrid1_grainPitch] = 12.0f;
    p[60].values[hybrid1_grainSize] = 0.15f;
    set(61, "Sample Prism", "Experimental", "Original engine study / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[61].values[hybrid1_mode] = 1.0f;
    p[61].values[hybrid1_reverse] = 1.0f;
    p[61].values[hybrid1_loop] = 1.0f;
    p[61].values[attack] = 0.3f;
    p[61].values[reverb_enabled] = 1.0f;
    set(62, "Velocity Atlas", "Experimental", "Original engine study / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[62].values[hybrid1_mode] = 2.0f;
    p[62].values[filter1_key] = 1.0f;
    p[62].values[delay_enabled] = 1.0f;
    set(63, "Split Band", "Experimental", "Original engine study / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[63].values[filter2_enabled] = 1.0f;
    p[63].values[filter_path] = 1.0f;
    p[63].values[filter_mix] = 0.5f;
    p[63].values[filter1_type] = 1.0f;
    p[63].values[cutoff] = 2200.0f;
    p[63].values[filter2_type] = 2.0f;
    p[63].values[filter2_cutoff] = 700.0f;
    set(64, "Phase Sculpture", "Experimental", "Original engine study / C2-C5",
        {.6f, .01f, .4f, .65f, .5f, 6500, .2f, .35f});
    p[64].values[osc2_level] = 0.5f;
    p[64].values[osc2_interaction] = 1.0f;
    p[64].values[osc2_interactionAmount] = 0.7f;
    p[64].values[osc3_level] = 0.3f;
    p[64].values[osc3_interaction] = 2.0f;
    p[64].values[osc3_interactionAmount] = 0.5f;
    p[64].values[mod5_source] = 6.0f;
    p[64].values[mod5_destination] = 10.0f;
    p[64].values[mod5_amount] = 0.3f;
#include "EDMPresets.inc"
    // Four audible macros for every patch. Defaults at zero preserve the base sound.
    for (auto &patch : p)
    {
        auto &v = patch.values;
        if (std::string_view(patch.category) == "Lead")
            patch.category = "Leads";
        if (std::string_view(patch.category) == "Pad")
            patch.category = "Pads";
        patch.macroNames = {"Brightness", v[hybrid1_mode] > 0 ? "Pitch bend" : "Table morph",
                            "Resonance", "Softness"};
        if (&patch >= p.data() + 65)
        {
            // Preserve the EDM bank tags.
        }
        else if (std::string_view(patch.category) == "Bass")
            patch.tags = "bass,low,original";
        else if (std::string_view(patch.category) == "Pads")
            patch.tags = "pad,sustained,original";
        else if (std::string_view(patch.category) == "Leads")
            patch.tags = "lead,expressive,original";
        else if (std::string_view(patch.category) == "Arp / Sequence")
            patch.tags = "pattern,sync,original";
        else if (std::string_view(patch.category) == "Experimental")
            patch.tags = "hybrid,experimental,original";
        else if (std::string_view(patch.category) == "Atmosphere")
            patch.tags = "texture,fx,original";
        else
            patch.tags = "keys,pluck,original";
        v[mod1_source] = 22;
        v[mod1_destination] = 16;
        v[mod1_amount] = .3f;
        v[mod2_source] = 23;
        v[mod2_destination] = v[hybrid1_mode] > 0 ? 2 : 1;
        v[mod2_amount] = v[hybrid1_mode] > 0 ? .04f : .35f;
        v[mod3_source] = 24;
        v[mod3_destination] = 17;
        v[mod3_amount] = .35f;
        v[mod4_source] = 25;
        v[mod4_destination] = 3;
        v[mod4_amount] = -.5f;
        if (v[hybrid1_mode] > 0)
        {
            v[mod8_source] = 18;
            v[mod8_destination] = 16;
            v[mod8_amount] = .15f;
        }
    }
    // Presentation categories do not change sound recipes or host program indices.
    for (auto &patch : p)
    {
        const std::string_view category(patch.category);
        if (category == "Keys" || category == "Pluck")
            patch.category = "Keys & Plucks";
        if (category == "Atmosphere")
            patch.category = "SFX";
        if (category == "Arp / Sequence")
            patch.category = "Arps & Sequences";
    }
    p[23].category = "Pads";
    p[24].category = "Leads";
    p[59].category = "Pads";
    p[60].category = "Pads";
    p[61].category = "Keys & Plucks";
    p[62].category = "Keys & Plucks";
    p[63].category = "Bass";
    p[64].category = "SFX";
    return p;
}();
inline int modulationTarget(size_t parameter)
{
    for (int o = 0; o < 3; ++o)
    {
        auto p = oscillatorIndices[size_t(o)];
        std::array<int, 5> ids{p.table, p.pitch, p.level, p.pan, p.warpAmount};
        for (int i = 0; i < 5; ++i)
            if (parameter == size_t(ids[size_t(i)]))
                return 1 + o * 5 + i;
    }
    if (parameter == cutoff)
        return 16;
    if (parameter == resonance)
        return 17;
    if (parameter == filter2_cutoff)
        return 18;
    if (parameter == filter2_res)
        return 19;
    if (parameter == sub_level)
        return 20;
    if (parameter == noise_level)
        return 21;
    return 0;
}
inline const char *choiceName(size_t id, int value)
{
    static constexpr std::array<const char *, 4> filters{"Low-pass", "High-pass", "Band-pass",
                                                         "Notch"};
    static constexpr std::array<const char *, 4> warps{"Off", "Pulse width", "Phase bend", "Sync"};
    static constexpr std::array<const char *, 3> interactions{"Off", "Phase mod", "Ring mod"};
    static constexpr std::array<const char *, 4> routing{"Filter 1", "Filter 2", "Both", "Bypass"};
    static constexpr std::array<const char *, 6> shapes{"Sine",   "Triangle", "Saw up",
                                                        "Square", "Saw down", "Custom"};
    static constexpr std::array<const char *, 9> divisions{
        "1/64", "1/32", "1/16", "1/8", "1/4", "1/4 dotted", "1/2", "1/2 dotted", "1 bar"};
    for (auto o : oscillatorIndices)
    {
        if (id == size_t(o.warp))
            return warps[size_t(std::clamp(value, 0, 3))];
        if (id == size_t(o.interaction))
            return interactions[size_t(std::clamp(value, 0, 2))];
        if (id == size_t(o.route))
            return routing[size_t(std::clamp(value, 0, 3))];
    }
    for (int o = 0; o < 3; ++o)
        if (id == size_t(osc1_warp2 + o * 2))
            return warps[size_t(std::clamp(value, 0, 3))];
    for (int o = 0; o < 3; ++o)
        if (id == size_t(hybrid1_mode + o * 18))
        {
            static constexpr std::array<const char *, 5> modes{"Wavetable", "Sample", "Multisample",
                                                               "Granular", "Spectral"};
            return modes[size_t(std::clamp(value, 0, 4))];
        }
    if (id == sub_route || id == noise_route)
        return routing[size_t(std::clamp(value, 0, 3))];
    if (id == filter1_type || id == filter2_type)
        return filters[size_t(std::clamp(value, 0, 3))];
    if (id >= lfo1_rate && id < macro1)
    {
        int offset = int(id - lfo1_rate) % 8;
        if (offset == 1)
            return shapes[size_t(std::clamp(value, 0, 5))];
        if (offset == 3)
            return divisions[size_t(std::clamp(value, 0, 8))];
    }
    if (id == arp_division || id == seq_division)
        return divisions[size_t(std::clamp(value, 0, 8))];
    if (id == arp_order)
    {
        static constexpr std::array<const char *, 5> orders{"Up", "Down", "Up/down", "Random",
                                                            "As played"};
        return orders[size_t(std::clamp(value, 0, 4))];
    }
    if (id == delay_division)
        return divisions[size_t(std::clamp(value, 0, 8))];
    if (id >= mod1_source && id < sat_enabled)
    {
        int offset = int(id - mod1_source) % 6;
        if (offset == 0)
            return sourceNames[size_t(std::clamp(value, 0, 29))];
        if (offset == 1)
            return destinationNames[size_t(std::clamp(value, 0, 21))];
    }
    if (id >= fx_slot0 && id <= fx_slot7)
        return effectNames[size_t(std::clamp(value, 0, 7))];
    if (id == sub_wave)
    {
        static constexpr std::array<const char *, 3> names{"Sine", "Triangle", "Saw"};
        return names[size_t(std::clamp(value, 0, 2))];
    }
    if (id == filter_path)
        return value ? "Parallel" : "Serial";
    if (parameters[id].step == 1 && parameters[id].min == 0 && parameters[id].max == 1)
        return value ? "On" : "Off";
    return nullptr;
}

} // namespace prism
