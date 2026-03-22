#!/usr/bin/env python3
import bt2
import time

# Note: switched to host Lars!
LIVE_URL = "net://192.168.0.105/host/raspberrypi/session-live"
# LIVE_URL = "net://localhost/host/noether/scan-session2"

print(f"Connecting to LTTng live stream: {LIVE_URL}")

# Create iterator
it = bt2.TraceCollectionMessageIterator(LIVE_URL)

while True:
    try:
        # Try to fetch next message
        msg = next(it)

    except bt2.TryAgain:
        # No data yet — wait and retry
        time.sleep(0.1)
        continue

    except StopIteration:
        print("Stream ended.")
        break

    # Process only event messages
    if type(msg) is bt2._EventMessageConst:
        ev = msg.event
        payload = ev.payload_field

        print(f"\n=== Event: {ev.name} ===")
        for name, val in payload.items():
            print(f"{name}: {val}")

            # DO SOMETHING WITH THE LOGGED DATA