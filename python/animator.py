import time

# Check for monotonic method
# create a version when not running on CircuitPython hardware
try:
    getattr(time, 'monotonic')
except Exception:
    start_time = time.time()
    time.monotonic = (lambda: time.time() - start_time)


class Animator():
    """
    Class to help create non-blocking LED animations
    Use the start_animation method to add animation functions that
    use yield to return how long they should be delayed for between parts (in seconds)

    Then be sure to call update in your code's inner loop
    Example:

    def drawX():
        # draw first part
        yield 0.1  # wait 1/10th of a second
        # draw second part
        yield 0.2  # wait 1/5th of a second
        # draw third part

    animator = Animator()
    animator.start_animation(drawX)

    while True:
        animator.update()
    """
    def __init__(self):
        self.animations = []

    def start_animation(self, f, callback=None):
        now = time.monotonic()
        gen = f()
        self.animations.append((gen, callback, None, now))

    def update(self):
        now = time.monotonic()
        next_animations = []
        for animation in self.animations:
            gen, callback, delay, last_time = animation
            if delay is None or now - last_time > delay:
                try:
                    delay = next(gen)
                    next_animations.append((gen, delay, now))
                except StopIteration:
                    if callback is not None:
                        callback()
            else:
                next_animations.append(animation)

        self.animations = next_animations