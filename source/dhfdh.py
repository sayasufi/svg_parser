import matplotlib.pyplot as plt
import matplotlib.patches as patches

fig, ax = plt.subplots()

center = (8.388197960907203, 8.434953638656587)
radius = 4
start_angle = 179.3801
end_angle = -88.9852724202236

arc = patches.Arc(center, radius*2, radius*2, angle=0, theta1=start_angle, theta2=end_angle)
ax.add_patch(arc)

plt.xlim(0, 10)
plt.ylim(0, 10)
plt.grid()
plt.gca().set_aspect('equal', adjustable='box')
plt.show()
