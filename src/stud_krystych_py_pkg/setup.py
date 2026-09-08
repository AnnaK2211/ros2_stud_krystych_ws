from setuptools import find_packages, setup

package_name = 'stud_krystych_py_pkg'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='user',
    maintainer_email='ankristich@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'stud_krystych_subscriber = stud_krystych_py_pkg.student_subscriber:main',
            'stud_krystych_temperature_monitor = stud_krystych_py_pkg.temperature_monitor:main',
        ],
    },
)
