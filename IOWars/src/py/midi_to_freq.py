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

def midi_note_to_freq(note):
    return 440.0 * (2 ** ((note - 69) / 12.0))  # A4 = 440 Hz

def convert_midi(file_path):
    # Die API und Dateipfadsuchen bleiben exakt gleich
    try:
        mid = mido.MidiFile(file_path)
    except:
        file_path = "../IOWars/" + file_path
        mid = mido.MidiFile(file_path)

    ticks_per_beat = mid.ticks_per_beat
    events = []

    # Alle Spuren chronologisch zusammenführen
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

    events.sort(key=lambda x: x)

    processed_events = []
    current_tempo = 500000  # Default: 120 BPM
    last_tick = 0
    current_time_ms = 0.0

    # Exakte Umrechnung der Event-Zeitachsen in Millisekunden
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

    held_notes = []  # Liste aller aktuell aktiven Töne
    output = []
    last_time = 0.0

    # Zeitabschnitte linear verarbeiten (Monofonisierung via Skyline-Filter)
    for event in processed_events:
        event_time, event_type, note, velocity = event
        raw_duration = event_time - last_time
        
        if raw_duration > 0:
            if held_notes:
                # SKYLINE-ALGORITHMUS: 
                # Wenn mehrere Noten aktiv sind, nimm die mit der höchsten MIDI-Nummer (Melodieführung)
                highest_note = max(held_notes)
                output.append((midi_note_to_freq(highest_note), raw_duration))
            else:
                # Echte Pause (Stille) erfassen
                output.append((0.0, raw_duration))
                
        if event_type == 'on':
            held_notes.append(note)
        elif event_type == 'off':
            if note in held_notes:
                held_notes.remove(note)
        last_time = event_time

    # Letzten ausklingenden Ton verarbeiten, falls am Ende noch Noten aktiv waren
    if held_notes:
        duration = 100.0  # 100 ms standard tail
        highest_note = max(held_notes)
        output.append((midi_note_to_freq(highest_note), duration))

    # Datei schreiben im identischen Format (.freq)
    base_name, ext = os.path.splitext(file_path)
    output_file = base_name + f".freq"

    with open(output_file, "w") as f:
        for freq, duration in output:
            if duration > 0:
                f.write(f"{freq:.2f} {int(duration)}\n")

    print(f"Converted (Monophonic Skyline-Filter) {file_path} -> {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python midi_to_freq.py <midi_file>")
        filepath = input("Enter Filepath manually: ")
    else:
        filepath = sys.argv[1]

    print(f"Filepath: {filepath}")
    print("Converting file (Extracting Lead Melody)...")
    convert_midi(filepath)
    print("Done!")
