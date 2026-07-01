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
tremolo_period_ms = 100  # Change this value to set tremolo switch rate (e.g., 30 for 30ms)
use_velocity_weights = True  # Set to False to split durations equally among held notes

def midi_note_to_freq(note):
    return 440.0 * (2 ** ((note - 69) / 12.0))  # A4 = 440 Hz

def extract_initial_tempo(mid):
    tempo = 500000
    for track in mid.tracks:
        for msg in track:
            if msg.type == 'set_tempo':
                tempo = msg.tempo
                return tempo
    return tempo

def convert_midi(file_path):
    duration_multiplier = 0.9
    try:
        mid = mido.MidiFile(file_path)
    except:
        file_path = "../IOWars/" + file_path
        mid = mido.MidiFile(file_path)

    tempo = extract_initial_tempo(mid)
    ticks_per_beat = mid.ticks_per_beat

    # Collect all note events (note, on/off, time, velocity)
    events = []
    current_time = 0.0
    for msg in mid:
        if msg.type == 'set_tempo':
            tempo = msg.tempo
        delta_time = mido.tick2second(msg.time, ticks_per_beat, tempo) * 1_000_000
        current_time += delta_time
        if msg.type == 'note_on' and msg.velocity > 0:
            events.append((current_time, 'on', msg.note, msg.velocity))
        elif msg.type == 'note_off' or (msg.type == 'note_on' and msg.velocity == 0):
            events.append((current_time, 'off', msg.note, 0))

    events.sort()

    held_notes = []  # List of tuples: (note, velocity)
    output = []
    last_time = 0.0

    period_us = tremolo_period_ms * 1000  # tremolo period in microseconds

    for event in events:
        event_time, event_type, note, velocity = event
        duration = (event_time - last_time) * duration_multiplier
        if held_notes and duration > 0:
            t = 0.0
            while t < duration:
                slice_duration = min(period_us, duration - t)
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
        if event_type == 'on':
            held_notes.append((note, velocity))
        elif event_type == 'off':
            held_notes = [(n, v) for (n, v) in held_notes if n != note]
        last_time = event_time

    # Handle any remaining notes at the end
    if held_notes:
        duration = 100_000  # 100 ms tail
        t = 0.0
        while t < duration:
            slice_duration = min(period_us, duration - t)
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

    base_name, ext = os.path.splitext(file_path)
    output_file = base_name + f".freq"

    with open(output_file, "w") as f:
        for freq, duration in output:
            f.write(f"{freq} {duration}\n")

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
