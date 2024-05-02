import matplotlib.pyplot as plt
import matplotlib.patches as patches

fig, ax = plt.subplots()

center = (0, 0)
radius = 1
start_angle = 179.3801
end_angle = -134.704568

arc = patches.Arc(center, radius*2, radius*2, angle=0, theta1=start_angle, theta2=end_angle)
ax.add_patch(arc)

plt.xlim(-2, 2)
plt.ylim(-2, 2)
plt.gca().set_aspect('equal', adjustable='box')
plt.show()
