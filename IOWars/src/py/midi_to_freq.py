import mido
import sys
import os

# MIDI note to frequency conversion
def midi_note_to_freq(note):
    return 440.0 * (2 ** ((note - 69) / 12.0))  # A4 = 440 Hz

def convert_midi(file_path):
    duration_multiplier = 0.9  # Increase tempo by 10%

    try:
        mid = mido.MidiFile(file_path)
    except:
        file_path = "../IOWars/" + file_path
        mid = mido.MidiFile(file_path)

    events = []  # Stores (frequency, duration)
    active_notes = {}  # {note: start_time}
    tempo = 500000  # Default 120 BPM (500,000 µs per beat)
    ticks_per_beat = mid.ticks_per_beat
    current_time = 0.0  # Time in microseconds
    last_event_time = 0.0  # Last processed event time

    for msg in mid:
        # Convert MIDI ticks to real time in microseconds
        delta_time = mido.tick2second(msg.time, ticks_per_beat, tempo) * 1_000_000
        current_time += delta_time

        if msg.type == 'set_tempo':
            tempo = msg.tempo  # Update tempo dynamically

        elif msg.type == 'note_on' and msg.velocity > 0:  # Note On
            if not active_notes and current_time > last_event_time:  # Detect silence
                silence_duration = current_time - last_event_time
                events.append((0, silence_duration))  # Store silence period

            active_notes[msg.note] = current_time  # Store note start time

        elif msg.type in ('note_off', 'note_on') and msg.note in active_notes:  # Note Off or Note On with velocity 0
            start_time = active_notes.pop(msg.note)
            note_duration = current_time - start_time

            if note_duration > 0:
                # If multiple notes were active, divide duration equally
                num_notes = len(active_notes) + 1  # Include the current note
                adjusted_duration = (note_duration * duration_multiplier) / num_notes

                # Store frequencies of all active notes
                for note in list(active_notes.keys()) + [msg.note]:
                    events.append((midi_note_to_freq(note), adjusted_duration))

            if not active_notes:  # If no active notes, update last event time
                last_event_time = current_time

    # Save frequency-duration pairs to file
    base_name, ext = os.path.splitext(file_path)
    output_file = base_name + ".freq"

    with open(output_file, "w") as f:
        for freq, duration in events:
            f.write(f"{freq} {duration}\n")

    print(f"Converted {file_path} -> {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python midi_to_freq.py <midi_file>")
        filepath = input("Enter Filepath manually: ")
    else:
        filepath = sys.argv[1]

    print(f"Filepath: {filepath}")
    print("Converting file...")
    convert_midi(filepath)
    print("Done!")
