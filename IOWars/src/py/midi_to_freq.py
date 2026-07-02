try:
    import mido
except ImportError:
    print("mido library not found. Installing mido...")
    import pip
    pip.main(['install', 'mido'])
    import mido
    print("mido successfully imported after installation")

import sys
import os

# --- CONFIGURABLES ---
tremolo_period_ms = 20  # Tremolo switch rate in milliseconds
use_velocity_weights = True  # Set to False to split tremolo durations equally among held notes

def midi_note_to_freq(note):
    return 440.0 * (2 ** ((note - 69) / 12.0))  # A4 = 440 Hz

def convert_midi(file_path):
    duration_multiplier = 1
    try:
        mid = mido.MidiFile(file_path)
    except:
        file_path = "../IOWars/" + file_path
        mid = mido.MidiFile(file_path)

    ticks_per_beat = mid.ticks_per_beat
    events = []

    for track in mid.tracks:
        current_tick = 0
        for msg in track:
            current_tick += msg.time
            if msg.type == 'set_tempo':
                events.append((current_tick, 'tempo', msg.tempo, 0))
            elif msg.type == 'note_on' and msg.velocity > 0:
                events.append((current_tick, 'on', msg.note, msg.velocity))
            elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
                events.append((current_tick, 'off', msg.note, 0))

    events.sort(key=lambda x: x[0])

    processed_events = []
    current_tempo = 500000  # Default: 120 BPM
    last_tick = 0
    current_time_ms = 0.0

    for tick, e_type, note, velocity in events:
        delta_ticks = tick - last_tick
        if delta_ticks > 0:
            delta_time_ms = mido.tick2second(delta_ticks, ticks_per_beat, current_tempo) * 1_000
            current_time_ms += delta_time_ms
        
        if e_type == 'tempo':
            current_tempo = note
        else:
            processed_events.append((current_time_ms, e_type, note, velocity))
            
        last_tick = tick

    held_notes = []  # List of tuples: (note, velocity)
    output = []
    last_time = 0.0

    period_ms = tremolo_period_ms

    for event in processed_events:
        event_time, event_type, note, velocity = event
        raw_duration = event_time - last_time
        
        if raw_duration > 0:
            if held_notes:
                duration = raw_duration * duration_multiplier
                silence_gap = raw_duration - duration
                
                # FIX: Tremolo NUR anwenden, wenn MEHR ALS EINE Note gehalten wird
                if len(held_notes) > 1:
                    t = 0.0
                    while t < duration:
                        slice_duration = min(period_ms, duration - t)
                        if use_velocity_weights:
                            velocities = [v for (_, v) in held_notes]
                            total_velocity = sum(velocities)
                            if total_velocity == 0:
                                per_note_duration = slice_duration / len(held_notes)
                                for n, v in held_notes:
                                    output.append((midi_note_to_freq(n), per_note_duration))
                            else:
                                for n, v in held_notes:
                                    note_duration = slice_duration * (v / total_velocity)
                                    output.append((midi_note_to_freq(n), note_duration))
                        else:
                            per_note_duration = slice_duration / len(held_notes)
                            for n, v in held_notes:
                                output.append((midi_note_to_freq(n), per_note_duration))
                        t += slice_duration
                else:
                    # Wenn nur eine einzige Note aktiv ist, spiele sie ohne Tremolo-Slicing durch
                    single_note, _ = held_notes[0]
                    output.append((midi_note_to_freq(single_note), duration))
                
                if silence_gap > 0:
                    output.append((0.0, silence_gap))
            else:
                output.append((0.0, raw_duration))
                
        if event_type == 'on':
            held_notes.append((note, velocity))
        elif event_type == 'off':
            held_notes = [(n, v) for (n, v) in held_notes if n != note]
        last_time = event_time

    # Letzte gehaltene Noten ausfaden
    if held_notes:
        duration = 100.0  # 100 ms tail
        if len(held_notes) > 1:
            t = 0.0
            while t < duration:
                slice_duration = min(period_ms, duration - t)
                if use_velocity_weights:
                    velocities = [v for (_, v) in held_notes]
                    total_velocity = sum(velocities)
                    if total_velocity == 0:
                        per_note_duration = slice_duration / len(held_notes)
                        for n, v in held_notes:
                            output.append((midi_note_to_freq(n), per_note_duration))
                    else:
                        for n, v in held_notes:
                            note_duration = slice_duration * (v / total_velocity)
                            output.append((midi_note_to_freq(n), note_duration))
                else:
                    per_note_duration = slice_duration / len(held_notes)
                    for n, v in held_notes:
                        output.append((midi_note_to_freq(n), per_note_duration))
                t += slice_duration
        else:
            single_note, _ = held_notes[0]
            output.append((midi_note_to_freq(single_note), duration))

    base_name, ext = os.path.splitext(file_path)
    output_file = base_name + f".freq"

    with open(output_file, "w") as f:
        for freq, duration in output:
            if duration > 0:
                f.write(f"{freq:.2f} {int(duration)}\n")

    print(f"Converted (tremolo, {'velocity-weighted' if use_velocity_weights else 'equal'}, {tremolo_period_ms}ms) {file_path} -> {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python midi_to_freq.py <midi_file>")
        filepath = input("Enter Filepath manually: ")
    else:
        filepath = sys.argv[1]

    print(f"Filepath: {filepath}")
    print(f"Tremolo period: {tremolo_period_ms} ms")
    print(f"Velocity weighting: {'on' if use_velocity_weights else 'off'}")
    print("Converting file...")
    convert_midi(filepath)
    print("Done!")
